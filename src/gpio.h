/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>


// Student Edit: Define these, 0's are placeholder values.
// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.

#define	LED0_port  gpioPortF
#define LED0_pin   4
#define LED1_port  gpioPortF
#define LED1_pin   5
#define PB0_PORT   gpioPortF
#define PB0_PIN    6
#define PB1_PORT   gpioPortF
#define PB1_PIN    7

#define SI7021_PORT         gpioPortD
#define SI7021_PIN          15
#define DISP_PORT           gpioPortD
#define DISP_PIN            13






#define LIGHT_STATE_PORT           gpioPortC
#define LIGHT_STATE_PIN            9           // EXP10

#define MOTION_DETECT_PORT         gpioPortA
#define MOTION_DETECT_PIN          3           // EXP5


#define CAR_LOT_PORT               gpioPortD
#define CAR_LOT_PIN                10           // EXP7



// Function prototypes
void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void si7021_enable();
void si7021_disable();
void lcd_enable();
void lcd_disable();
void leds_on();
void leds_off();
void gpioSensorEnSetOn();
void gpioSetDisplayExtcomin(bool status);
void light_sensor_enable();
void light_sensor_disable();
void light_state_Off();
void light_state_On();
void motion_state_Off();
void motion_state_On();
void car_lot_Off();
void car_lot_On();
void ToggleLED0 (void);


#endif /* SRC_GPIO_H_ */
