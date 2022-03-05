/**
 * @brief Example usage of Timer peripheral.
 *
 * EFR32 Application Note on Timers
 * https://www.silabs.com/documents/public/application-notes/AN0014.pdf
 *
 * EFR32MG12 Wireless Gecko Reference Manual (Timer p672)
 * https://www.silabs.com/documents/public/reference-manuals/efr32xg12-rm.pdf
 *
 * Timer API documentation 
 * https://docs.silabs.com/mcu/latest/efr32mg12/group-TIMER
 * 
 * ARM RTOS API
 * https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS.html
 * 
 * Copyright Thinnect Inc. 2019
 * Copyright ProLab TTÜ 2022
 * @license MIT
 * @author Johannes Ehala
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"
#include "cmsis_os2.h"
#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "loggers_ext.h"
#include "logger_fwrite.h"

#include "timer_handler.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

// Heartbeat thread, initialize Timer and print heartbeat messages.
void leds_loop();
void hp_loop ()
{
    #define ESWGPIO_HB_DELAY 10 // Heartbeat message delay, seconds
    uint32_t timer_freq;
    // Initialize Timer.
    timer_freq = timer0_init();
    // Initialize LEDs
    leds_gpio_init();

    // Create a thread for LEDs.
    const osThreadAttr_t leds_thread_attr = { .name = "led" };
    osThreadNew(leds_loop, NULL, &leds_thread_attr);
    
    for (;;)
    {
        osDelay(ESWGPIO_HB_DELAY*osKernelGetTickFreq());
        info1("Heartbeat");
    }
}

// Responsible for dimming LEDs fade in/out. 
// Red -> 40 Green -> 100 Blue -> 20 seems the equal brightness
// To obtain that values, a loop created and increased/decreased
// from 0-20 and 20-0 by multipliers with 2 for Red, 5 for Green
// and 1 for Blue. FADE_DELAY sets the time period for fade in/out
// and SLEEP_DELAY keeps the max/min brightness between fade in/out
void leds_loop(void *args)
{
    #define FADE_DELAY 20 // ms
    #define SLEEP_DELAY 50 // ms

    uint32_t i;

    for (;;)
    {
        for(i=0; i<20; ++i)
        {
            duty_cycle(i*2, i*5, i);
            osDelay(FADE_DELAY*osKernelGetTickFreq()/1000);
        }

        osDelay(SLEEP_DELAY*osKernelGetTickFreq()/1000);
        for(i=20; i>0; i--)
        {
            duty_cycle(i*2, i*5, i);
            osDelay(FADE_DELAY*osKernelGetTickFreq()/1000);
        }

        duty_cycle(0, 0, 0);
        osDelay(SLEEP_DELAY*osKernelGetTickFreq()/1000);
    }
}

int logger_fwrite_boot (const char *ptr, int len)
{
    fwrite(ptr, len, 1, stdout);
    fflush(stdout);
    return len;
}

int main ()
{
    PLATFORM_Init();

    // Configure log message output
    RETARGET_SerialInit();
    log_init(BASE_LOG_LEVEL, &logger_fwrite_boot, NULL);

    info1("ESW-Timer "VERSION_STR" (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel.
    osKernelInitialize();

    // Create a thread.
    const osThreadAttr_t hp_thread_attr = { .name = "hp" };
    osThreadNew(hp_loop, NULL, &hp_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        // Switch to a thread-safe logger
        logger_fwrite_init();
        log_init(BASE_LOG_LEVEL, &logger_fwrite, NULL);

        // Start the kernel
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for(;;);
}
