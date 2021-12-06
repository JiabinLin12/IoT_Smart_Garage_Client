/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.

 */




#include "gpio.h"





// Set GPIO drive strengths and modes of operation
void gpioInit()
{

	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);

	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthStrongAlternateStrong);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

  GPIO_DriveStrengthSet(LIGHT_STATE_PORT, gpioDriveStrengthStrongAlternateStrong);
  GPIO_PinModeSet(LIGHT_STATE_PORT, LIGHT_STATE_PIN, gpioModePushPull, false);

  GPIO_DriveStrengthSet(MOTION_DETECT_PORT, gpioDriveStrengthStrongAlternateStrong);
  GPIO_PinModeSet(MOTION_DETECT_PORT, MOTION_DETECT_PIN, gpioModePushPull, false);

  GPIO_DriveStrengthSet(CAR_LOT_PORT, gpioDriveStrengthStrongAlternateStrong);
  GPIO_PinModeSet(CAR_LOT_PORT, CAR_LOT_PIN, gpioModePushPull, false);

  //PB0
  GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInputPullFilter, true);
  GPIO_ExtIntConfig(PB0_PORT,PB0_PIN,PB0_PIN,true,false,true);

  //PB1
  GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInputPullFilter, true);
  GPIO_ExtIntConfig(PB1_PORT,PB1_PIN,PB1_PIN,true,false,true);

  NVIC_EnableIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

} // gpioInit()


void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}


void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}


void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}


void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void light_state_Off()
{
  GPIO_PinOutClear(LIGHT_STATE_PORT,LIGHT_STATE_PIN);
}


void light_state_On()
{
  GPIO_PinOutSet(LIGHT_STATE_PORT,LIGHT_STATE_PIN);
}

void motion_state_Off(){
  GPIO_PinOutClear(MOTION_DETECT_PORT,MOTION_DETECT_PIN);
}

void motion_state_On()
{
  GPIO_PinOutSet(MOTION_DETECT_PORT,MOTION_DETECT_PIN);
}

void car_lot_Off(){
  GPIO_PinOutClear(CAR_LOT_PORT,CAR_LOT_PIN);
}

void car_lot_On()
{
  GPIO_PinOutSet(CAR_LOT_PORT,CAR_LOT_PIN);
}

/**
 * Enable SENSOR_ENABLE PIN for si7021 in the schematic
 * to enable the sensor
 * */
void si7021_enable(){
  GPIO_PinOutSet(SI7021_PORT,SI7021_PIN);
}


void lcd_enable(){
  GPIO_PinOutSet(SI7021_PORT,SI7021_PIN);
}


void lcd_disable(){
  GPIO_PinOutClear(SI7021_PORT,SI7021_PIN);
}
/**
 * Disable SENSOR_ENABLE PIN for si7021 in the schematic
 * to disable the sensor
 * */
void si7021_disable(){
  GPIO_PinOutClear(SI7021_PORT,SI7021_PIN);
}

void gpioSensorEnSetOn(){
  GPIO_PinOutSet(DISP_PORT,DISP_PIN);
}

void gpioSetDisplayExtcomin(bool status){
  if(status){
    GPIO_PinOutSet(DISP_PORT,DISP_PIN);
  }else{
    GPIO_PinOutClear(DISP_PORT,DISP_PIN);
  }
}


void ToggleLED0 () {
  static bool  state=false;

  if (state) {
      state = false;
      gpioLed0SetOff();
  } else {
      state = true;
      gpioLed0SetOn();
  }
}


void leds_on(){
  gpioLed0SetOn();
  gpioLed1SetOn();
}

void leds_off(){
  gpioLed0SetOff();
  gpioLed1SetOff();
}
