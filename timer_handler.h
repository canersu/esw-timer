/**
 * @file timer_handler.h
 *
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright ProLab, TTÜ. 15 January 2022
 */

#ifndef TIMER_HANDLER_H_
#define TIMER_HANDLER_H_

// Public functions
uint32_t timer0_init();
void duty_cycle(uint32_t val_red, uint32_t val_green, uint32_t val_blue);
void leds_gpio_init();

#endif // TIMER_HANDLER_H_ */
