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

/* Create ACLs for main box. */
MAIN_ACL(g_main_acl);

/* Enable uVisor. */
UVISOR_SET_MODE_ACL(UVISOR_ENABLED, g_main_acl);
UVISOR_SET_PAGE_HEAP(8*1024, 1);

/* FIXME: Add missing uVisor box .bss due to alignment */
#if defined(TARGET_NUMAKER_PFM_M487)
uint8_t __attribute__((section(".keep.uvisor.bss.boxes"), aligned(32))) __boxes_overhead[40832];
#endif

int main(void)
{
    DigitalOut led1(MAIN_LED);

    printf("\r\n***** NuMaker mbed uVisor example *****\r\n");

    size_t count = 0;

    while (1)
    {
        printf("Main loop count: %d\r\n", count ++);
        led1 = ! led1;

        /* Blink once per second */
        Thread::wait(500);
    }

    return 0;
}
