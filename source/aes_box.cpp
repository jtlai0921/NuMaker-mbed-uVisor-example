/* mbed Microcontroller Library
 * Copyright (c) 2015-2016 Nuvoton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "uvisor-lib/uvisor-lib.h"
#include "mbed.h"
#include "main-hw.h"
#include "mbedtls/aes.h"

typedef struct  {
    mbedtls_aes_context aes_ctx;
    unsigned char       aes_key[32];
    int                 caller_id;
    RawSerial *         serial;
} aes_box_context;

static const UvisorBoxAclItem acl[] = {
    /* This secure box is pure software; no secure peripherals are required. */
    {CRPT,                          sizeof(*CRPT),     UVISOR_TACLDEF_PERIPH}
};

static void aes_box_main(const void *);
static void aes_box_encrypt_run(void);
static void aes_box_decrypt_run(void);
/* RPC target functions */
static int aes_box_encrypt_cbc(size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);
static int aes_box_decrypt_cbc(size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output);

/* Box configuration */
UVISOR_BOX_NAMESPACE(NULL);
UVISOR_BOX_HEAPSIZE(8192);
UVISOR_BOX_MAIN(aes_box_main, osPriorityNormal, UVISOR_BOX_STACK_SIZE);
UVISOR_BOX_CONFIG(aes_box, acl, UVISOR_BOX_STACK_SIZE, aes_box_context);

#ifdef __uvisor_ctx
#define uvisor_ctx ((aes_box_context *) __uvisor_ctx)
#endif

/* RPC gateways */
UVISOR_BOX_RPC_GATEWAY_SYNC(aes_box, secure_aes_encrypt_cbc, aes_box_encrypt_cbc, int, size_t, unsigned char iv[16], const unsigned char *, unsigned char *);
UVISOR_BOX_RPC_GATEWAY_ASYNC(aes_box, secure_aes_encrypt_cbc_async, aes_box_encrypt_cbc, int, size_t, unsigned char iv[16], const unsigned char *, unsigned char *);
UVISOR_BOX_RPC_GATEWAY_SYNC(aes_box, secure_aes_decrypt_cbc, aes_box_decrypt_cbc, int, size_t, unsigned char iv[16], const unsigned char *, unsigned char *);
UVISOR_BOX_RPC_GATEWAY_ASYNC(aes_box, secure_aes_decrypt_cbc_async, aes_box_decrypt_cbc, int, size_t, unsigned char iv[16], const unsigned char *, unsigned char *);

unsigned char (*aes_key_disclosed)[32];

/* NOTE: AES encrypt/decrypt threads call into mbedtls_aes_xxx functions which need 2KB more stack size. */
#define AES_THREAD_STACK_SIZE   4096
    
#if (osCMSIS >= 0x20000U)
/* A thin wrapper around aes_box_decrypt_run that accepts and ignores a context. This avoids a
 * cast, as mbed's Thread and RTX's osThreadContextNew operate on differently
 * typed thread functions. */
static void aes_box_decrypt_run_context(void *)
{
    aes_box_decrypt_run();
}
#endif

static void aes_box_main(const void *)
{
    /* Allocate serial port to ensure that code in this secure box won't touch
     * the handle in the default security context when printing. */
    uvisor_ctx->serial = new RawSerial(USBTX, USBRX);
    if (! uvisor_ctx->serial) {
        return;
    }

    mbedtls_aes_init(&uvisor_ctx->aes_ctx);
    
    /* FIXME: Replace with the use of a HW-RNG. */
    memset(uvisor_ctx->aes_key, 0xAB, sizeof (uvisor_ctx->aes_key));
    
    /* Disclose AES key purposely to demo guard from leakage */
    aes_key_disclosed = &(uvisor_ctx->aes_key);

    /* Create AES encrypt thread: heap/stack from statically allocated process heap  */
    Thread * thread_encrypt = new Thread(osPriorityNormal, AES_THREAD_STACK_SIZE);
    osStatus status = thread_encrypt->start(aes_box_encrypt_run);
    if (status != osOK) {
        printf("Could not start thread: aes_box_encrypt_run.\r\n");
        uvisor_error(USER_NOT_ALLOWED);
    }
    
    /* Create AES decrypt thread: heap/stack from page-backed allocator */
    /* Create page-backed allocator. */
    SecureAllocator alloc = secure_allocator_create_with_pages(8 * 1024, 1024);
    if (! alloc) {
        printf("secure_allocator_create_with_pages FAILED\r\n");
        uvisor_error(USER_NOT_ALLOWED);
    }
    /* Prepare the thread definition structure. */
#if (osCMSIS >= 0x20000U)
    osThreadAttr_t thread_attr = {0};
    os_thread_t thread_def = {0};
    thread_attr.stack_size = AES_THREAD_STACK_SIZE;
    /* Allocate the stack inside the page allocator! */
    thread_attr.stack_mem = (uint32_t *) secure_malloc(alloc, AES_THREAD_STACK_SIZE);
    thread_attr.priority = osPriorityNormal;
    thread_attr.cb_size = sizeof(thread_def);
    thread_attr.cb_mem = &thread_def;
    /* Create a thread with the page allocator as heap. */
    osThreadContextNew(&aes_box_decrypt_run_context, NULL, &thread_attr, alloc);
#else
    osThreadDef_t thread_def;
    thread_def.stacksize = AES_THREAD_STACK_SIZE;
    /* Allocate the stack inside the page allocator! */
    thread_def.stack_pointer = (uint32_t *) secure_malloc(alloc, AES_THREAD_STACK_SIZE);
    thread_def.tpriority = osPriorityNormal;
    thread_def.pthread = (void (*)(const void *)) &aes_box_decrypt_run;
    /* Create a thread with the page allocator as heap. */
    osThreadContextCreate(&thread_def, NULL, alloc);
#endif
    
    size_t count = 0;
    
    while (1)
    {
        printf("aes_box_main loop count: %d\r\n", count ++);
       
        Thread::wait(500);
    }
}

static void aes_box_encrypt_run(void)
{
    /* The list of functions we are interested in handling RPC requests for */
    static const TFN_Ptr aes_fn_array[] = {
        (TFN_Ptr) aes_box_encrypt_cbc
    };

    while (1) {
        int status;

        /* NOTE: This serializes all access to AES enc/dec! */
        status = rpc_fncall_waitfor(aes_fn_array, sizeof (aes_fn_array) / sizeof (aes_fn_array[0]), &uvisor_ctx->caller_id, UVISOR_WAIT_FOREVER);

        if (status) {
            uvisor_ctx->serial->printf("rpc_fncall_waitfor() FAILED\r\n");
            uvisor_error(USER_NOT_ALLOWED);
        }
    }
}

static void aes_box_decrypt_run(void)
{
    /* The list of functions we are interested in handling RPC requests for */
    static const TFN_Ptr aes_fn_array[] = {
        (TFN_Ptr) aes_box_decrypt_cbc
    };

    while (1) {
        int status;

        /* NOTE: This serializes all access to AES enc/dec! */
        status = rpc_fncall_waitfor(aes_fn_array, sizeof (aes_fn_array) / sizeof (aes_fn_array[0]), &uvisor_ctx->caller_id, UVISOR_WAIT_FOREVER);

        if (status) {
            uvisor_ctx->serial->printf("rpc_fncall_waitfor() FAILED\r\n");
            uvisor_error(USER_NOT_ALLOWED);
        }
    }
}

static int aes_box_encrypt_cbc(size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output)
{
    mbedtls_aes_setkey_enc(&uvisor_ctx->aes_ctx, uvisor_ctx->aes_key, sizeof (uvisor_ctx->aes_key) * 8);
    
    return mbedtls_aes_crypt_cbc(&uvisor_ctx->aes_ctx, MBEDTLS_AES_ENCRYPT, length, iv, input, output);
}

static int aes_box_decrypt_cbc(size_t length, unsigned char iv[16], const unsigned char *input, unsigned char *output)
{
    mbedtls_aes_setkey_dec(&uvisor_ctx->aes_ctx, uvisor_ctx->aes_key, sizeof (uvisor_ctx->aes_key) * 8);
    
    return mbedtls_aes_crypt_cbc(&uvisor_ctx->aes_ctx, MBEDTLS_AES_DECRYPT, length, iv, input, output);
}
