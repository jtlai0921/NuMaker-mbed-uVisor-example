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
#include "aes_box.h"
#include "main-hw.h"

typedef struct {
    RawSerial *         serial;
} aes_client_box_context;

/* NOTE: Use global memory to be accessible by aes_box. If we allocate these buffers in aes_client_box_context or 
 *       through calls to malloc/new, MPU fault will be triggered as these buffers such allocated belong to aes_client_box 
 *       and are inaccessible by aes_box. */
unsigned char           iv[16];
unsigned char           iv2[16];
static unsigned char    buf_plain[64];
static unsigned char    buf_plain2[64];
static unsigned char    buf_cipher[64];

static const UvisorBoxAclItem acl[] = {
    /* This secure box is pure software; no secure peripherals are required. */
};

static void aes_client_box_main(const void *);
static int aes_client_box_encrypt_cbc(
                    size_t length,
                    unsigned char *iv,
                    const unsigned char *input,
                    unsigned char *output);
static int aes_client_box_decrypt_cbc(
                    size_t length,
                    unsigned char *iv,
                    const unsigned char *input,
                    unsigned char *output);
                    
/* Box configuration */
UVISOR_BOX_NAMESPACE(NULL);
UVISOR_BOX_HEAPSIZE(2048);
UVISOR_BOX_MAIN(aes_client_box_main, osPriorityNormal, UVISOR_BOX_STACK_SIZE);
UVISOR_BOX_CONFIG(aes_client_box, acl, UVISOR_BOX_STACK_SIZE, aes_client_box_context);

#ifdef __uvisor_ctx
#define uvisor_ctx ((aes_client_box_context *) __uvisor_ctx)
#endif

static void aes_client_box_main(const void *)
{
    /* Allocate serial port to ensure that code in this secure box won't touch
     * the handle in the default security context when printing. */
    uvisor_ctx->serial = new RawSerial(USBTX, USBRX);
    if (! uvisor_ctx->serial) {
        return;
    }
    
    size_t count = 0;
    
    /* The entire box code runs in its main thread. */
    while (1) {
            
        memset(iv , 0x25, sizeof (iv));
        memset(buf_plain, 0x33, sizeof (buf_plain));
        memset(buf_cipher, 0x00, sizeof (buf_cipher));
        memset(buf_plain2, 0x00, sizeof (buf_plain2));
    
        /* AES encrypt */
        memcpy(iv2, iv, sizeof (iv));
        aes_client_box_encrypt_cbc(sizeof (buf_plain), iv2, buf_plain, buf_cipher);
        
        /* AES decrypt */
         memcpy(iv2, iv, sizeof (iv));
        aes_client_box_decrypt_cbc(sizeof (buf_cipher), iv2, buf_cipher, buf_plain2);
        
        /* Check original plain text and plain text which goes through encrypt/decrypt run */
        if (memcmp(buf_plain, buf_plain2, sizeof (buf_plain))) {
            uvisor_ctx->serial->printf("AES compare FAILED: %d\r\n", count ++);
        }
        else {
            uvisor_ctx->serial->printf("AES compare OK: %d\r\n", count ++);
        }
        
        Thread::wait(1000);
    }
}

static int aes_client_box_encrypt_cbc(
                    size_t length,
                    unsigned char *iv,
                    const unsigned char *input,
                    unsigned char *output)
{
#if 1
    /* Secure AES encrypt synchronously */
    return secure_aes_encrypt_cbc(length, iv, input, output);
#else
    /* Secure AES encrypt asynchronously */
    uvisor_rpc_result_t result = secure_aes_encrypt_cbc_async(length, iv, input, output);
    /* Wait on the result synchronously. */
    uint32_t ret;
    int status = rpc_fncall_wait(result, UVISOR_WAIT_FOREVER, &ret);
    if (status) {
        return -1;
    }
    return (int) ret;
#endif
}

static int aes_client_box_decrypt_cbc(
                    size_t length,
                    unsigned char *iv,
                    const unsigned char *input,
                    unsigned char *output)
{
#if 0
    /* Secure AES decrypt synchronously */
    return secure_aes_decrypt_cbc(length, iv, input, output);
#else
    /* Secure AES decrypt asynchronously */
    uvisor_rpc_result_t result = secure_aes_decrypt_cbc_async(length, iv, input, output);
    /* Wait on the result synchronously. */
    uint32_t ret;
    int status = rpc_fncall_wait(result, UVISOR_WAIT_FOREVER, &ret);
    if (status) {
        return -1;
    }
    return (int) ret;
#endif
}
