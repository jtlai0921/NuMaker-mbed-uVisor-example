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
#ifndef __AES_BOX_H__
#define __AES_BOX_H__

#include "uvisor-lib/uvisor-lib.h"

/* Secure sync/async version of mbedtls_aes_crypt_cbc in encrypt mode with key internally kept */
UVISOR_EXTERN int (*secure_aes_encrypt_cbc)(
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output);
UVISOR_EXTERN uvisor_rpc_result_t (*secure_aes_encrypt_cbc_async)(
                                    size_t length,
                                    unsigned char iv[16],
                                    const unsigned char *input,
                                    unsigned char *output);
                    
/* Secure sync/async version of mbedtls_aes_crypt_cbc in decrypt mode with key internally kept */
UVISOR_EXTERN int (*secure_aes_decrypt_cbc)(
                    size_t length,
                    unsigned char iv[16],
                    const unsigned char *input,
                    unsigned char *output);
UVISOR_EXTERN uvisor_rpc_result_t (*secure_aes_decrypt_cbc_async)(
                                    size_t length,
                                    unsigned char iv[16],
                                    const unsigned char *input,
                                    unsigned char *output);

/* Disclose AES key purposely to demo guard from leakage */
extern unsigned char (*aes_key_disclosed)[32];

#endif  /* __AES_BOX_H__ */
