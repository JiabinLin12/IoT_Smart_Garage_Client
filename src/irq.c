/*
 * irq.c
 *
 *  Created on: Dec 5, 2021
 *      Author: jiabinlin
 */

#include "gpio.h"
#include "client_state_machine.h"
void GPIO_EVEN_IRQHandler(void){
  GPIO_IntClear(1<<PB0_PIN);
  if (GPIO_PinInGet(PB0_PORT,PB0_PIN)) {
      sl_bt_external_signal(pb0_pressed);
  }
}

void GPIO_ODD_IRQHandler(void){
  GPIO_IntClear(1<<PB1_PIN);
  if (GPIO_PinInGet(PB1_PORT,PB1_PIN)) {
      sl_bt_external_signal(pb1_pressed);
  }
}
