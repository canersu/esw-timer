/**
 * @file timer_handler.c
 *
 * @brief Init TIMER0 to control buzzer with PWM signal.
 * 
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright ProLab, TTÜ. 15 January 2022
 */

#include "em_cmu.h"
#include "em_timer.h"

#include "timer_handler.h"

/**
 * @brief Init TIMER0 to regulate PWM dutycycle. 
 */
uint32_t timer0_init(void)
{
	// Enable clocks.
    CMU_ClockEnable(cmuClock_TIMER0, true);

    // Init compare mode init and configuring.
    TIMER_InitCC_TypeDef ccInit = TIMER_INITCC_DEFAULT;
    ccInit.cofoa = timerOutputActionToggle;
    ccInit.mode = timerCCModePWM;
    
    TIMER_InitCC(TIMER0, 0, &ccInit);
    TIMER_InitCC(TIMER0, 1, &ccInit);
    TIMER_InitCC(TIMER0, 2, &ccInit);
    
    TIMER_TopSet(TIMER0, 100);
    TIMER_CompareSet(TIMER0, 0, 40);
    TIMER_CompareSet(TIMER0, 1, 100);
    TIMER_CompareSet(TIMER0, 2, 20);
    
    TIMER0->ROUTEPEN |= (TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTEPEN_CC1PEN | TIMER_ROUTEPEN_CC2PEN);
    // TIMER0->ROUTEPEN = 2; // TIMER_ROUTEPEN_CC0PEN
    //TIMER0->ROUTELOC0 = TIMER_ROUTELOC0_CC0LOC_LOC0; // = 0 
    TIMER0->ROUTELOC0 |= (TIMER_ROUTELOC0_CC0LOC_LOC7 | TIMER_ROUTELOC0_CC1LOC_LOC5 | TIMER_ROUTELOC0_CC2LOC_LOC3);
    // TIMER0->ROUTELOC0 = TIMER_ROUTELOC0_CC1LOC_LOC5;
    // TIMER0->ROUTEPEN |= (TIMER_ROUTEPEN_CC0PEN | TIMER_ROUTELOC0_CC0LOC_LOC7);


    // Timer general init and config.
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    timerInit.prescale = timerPrescale64;
    timerInit.enable = true;
    
    TIMER_Init(TIMER0, &timerInit);
    
    return (uint32_t) (CMU_ClockFreqGet(cmuClock_TIMER0) / (uint32_t) (0x01 << timerPrescale64));
}

void leds_gpio_init()
{
    // Turn on GPIO module
    CMU_ClockEnable(cmuClock_GPIO, true);
    
    // Set buzzer pin to push-pull mode
    GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 0); //loc1
    GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0); //loc1
    GPIO_PinModeSet(gpioPortB, 12, gpioModePushPull, 0); //loc1
}

void duty_cycle(uint32_t val_red, uint32_t val_green, uint32_t val_blue)
{
    // if(val >= 100){val=100;}
    // if(val <= 0){val=0;}
    TIMER_CompareBufSet(TIMER0, 0, val_red);
    TIMER_CompareBufSet(TIMER0, 1, val_green);
    TIMER_CompareBufSet(TIMER0, 2, val_blue);
}