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
#include "aes_box.h"

typedef struct {
    InterruptIn *   sw;
    DigitalOut *    led;
    RawSerial *     serial;
} snoop_box_context;

static const UvisorBoxAclItem acl[] = {
    /* This secure box is pure software, no secure peripherals are required. */
};

static void snoop_box_main(const void *);
static void snoop_box_irq(void);

UVISOR_BOX_NAMESPACE(NULL);
UVISOR_BOX_HEAPSIZE(2048);
UVISOR_BOX_MAIN(snoop_box_main, osPriorityNormal, UVISOR_BOX_STACK_SIZE);
UVISOR_BOX_CONFIG(snoop_box, acl, UVISOR_BOX_STACK_SIZE, snoop_box_context);

static void snoop_box_main(const void *)
{
    /* Allocate serial port to ensure that code in this secure box
     * won't touch handle in the default security context when printing */
    RawSerial *serial = NULL;    
    serial = new RawSerial(USBTX, USBRX);
    if (! serial) {
        return;
    }
    /* Remember serial driver for IRQ routine */
    uvisor_ctx->serial = serial;

    /* Allocate a box-specific LED */
    uvisor_ctx->led = new DigitalOut(SNOOP_LED);
    if (! uvisor_ctx->led) {
        serial->printf("ERROR: failed to allocate memories for LED\r\n");
    }
    else {
        /* Turn LED off by default */
        *uvisor_ctx->led = LED_OFF;

        /* Allocate a box-specific switch */
        uvisor_ctx->sw = new InterruptIn(SNOOP_BTN);
        if (! uvisor_ctx->sw) {
            serial->printf("ERROR: failed to allocate memories for SW\r\n");
        }
        else {
            /* Register handler for switch SNOOP_BTN */
            uvisor_ctx->sw->mode(PullUp);
            uvisor_ctx->sw->fall(snoop_box_irq);
            
            /* No problem to return here as everything is initialized */
            return;
        }

        delete uvisor_ctx->led;
    }
    delete serial;
}

static void snoop_box_irq(void)
{
    /* Turn LED on */
    *uvisor_ctx->led = LED_ON;

    /* Attempt to snoop secret */
    if (aes_key_disclosed) {
        uvisor_ctx->serial->printf("Attempting to read AES key...\r\n");
    
        uvisor_ctx->serial->printf("AES key[0-7] = 0x%02F 0x%02F 0x%02F 0x%02F 0x%02F 0x%02F 0x%02F 0x%02F\r\n", 
                                    (*aes_key_disclosed)[0], (*aes_key_disclosed)[1], (*aes_key_disclosed)[2], (*aes_key_disclosed)[3],
                                    (*aes_key_disclosed)[4], (*aes_key_disclosed)[5], (*aes_key_disclosed)[6], (*aes_key_disclosed)[7]);
        
        uvisor_ctx->serial->printf("Access granted!\r\n");
    }
}
