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
#ifndef __MAIN_HW_H__
#define __MAIN_HW_H__

#if   defined(TARGET_LIKE_FRDM_K64F)

#define LED_ON          false
#define LED_OFF         true

#define MAIN_LED        LED_BLUE
#define SNOOP_LED       LED_RED

#define SNOOP_BTN       SW2

#define MAIN_ACL(acl_list_name) \
    static const UvisorBoxAclItem acl_list_name[] = {     \
        {MCG,    sizeof(*MCG),    UVISOR_TACLDEF_PERIPH}, \
        {SIM,    sizeof(*SIM),    UVISOR_TACLDEF_PERIPH}, \
        {PORTB,  sizeof(*PORTB),  UVISOR_TACLDEF_PERIPH}, \
        {PORTC,  sizeof(*PORTC),  UVISOR_TACLDEF_PERIPH}, \
        {RTC,    sizeof(*RTC),    UVISOR_TACLDEF_PERIPH}, \
        {LPTMR0, sizeof(*LPTMR0), UVISOR_TACLDEF_PERIPH}, \
        {PIT,    sizeof(*PIT),    UVISOR_TACLDEF_PERIPH}, \
        {SMC,    sizeof(*SMC),    UVISOR_TACLDEF_PERIPH}, \
        {UART0,  sizeof(*UART0),  UVISOR_TACLDEF_PERIPH}, \
    }
    
#elif defined(TARGET_NUMAKER_PFM_M487)

#define LED_ON          false
#define LED_OFF         true

#define MAIN_LED        LED_BLUE
#define SNOOP_LED       LED_RED

#define SNOOP_BTN       SW2
    
#define MAIN_ACL(acl_list_name)                                                         \
    static const UvisorBoxAclItem acl_list_name[] = {                                   \
        {SYS,                           sizeof(*SYS),       UVISOR_TACLDEF_PERIPH},     \
        {CLK,                           sizeof(*CLK),       UVISOR_TACLDEF_PERIPH},     \
        {UART1,                         sizeof(*UART1),     UVISOR_TACLDEF_PERIPH},     \
        {GPA,                           sizeof(*GPA),       UVISOR_TACLDEF_PERIPH},     \
        {GPB,                           sizeof(*GPB),       UVISOR_TACLDEF_PERIPH},     \
        {GPC,                           sizeof(*GPC),       UVISOR_TACLDEF_PERIPH},     \
        {GPD,                           sizeof(*GPD),       UVISOR_TACLDEF_PERIPH},     \
        {GPE,                           sizeof(*GPE),       UVISOR_TACLDEF_PERIPH},     \
        {GPF,                           sizeof(*GPF),       UVISOR_TACLDEF_PERIPH},     \
        {GPG,                           sizeof(*GPG),       UVISOR_TACLDEF_PERIPH},     \
        {GPH,                           sizeof(*GPH),       UVISOR_TACLDEF_PERIPH},     \
        {(void *) GPIO_PIN_DATA_BASE,   0x1D0,              UVISOR_TACLDEF_PERIPH},     \
        {TIMER0,                        sizeof(*TIMER0),    UVISOR_TACLDEF_PERIPH},     \
        {TIMER1,                        sizeof(*TIMER1),    UVISOR_TACLDEF_PERIPH},     \
        {TIMER2,                        sizeof(*TIMER2),    UVISOR_TACLDEF_PERIPH},     \
        {TIMER3,                        sizeof(*TIMER3),    UVISOR_TACLDEF_PERIPH},     \
    }

#endif

#endif /* __MAIN_HW_H__ */
