/*
 * client_state_machine.h
 *
 *  Created on: Nov 23, 2021
 *      Author: 17207
 */

#ifndef SRC_CLIENT_STATE_MACHINE_H_
#define SRC_CLIENT_STATE_MACHINE_H_

#include <stdint.h>
#include "sl_bt_api.h"
#include "gatt_db.h"
#include "ble.h"
#include "sl_status.h"
#include "math.h"
#include "gpio.h"

typedef enum
{
  SCANNING,

  DISCOVER_SERVICES,

  MD_DISCOVER_CHARACTERISTICS,
  MD_ENABLE_INDICATIONS,

  LS_DISCOVER_CHARACTERISTICS,
  LS_ENABLE_INDICATIONS,

  CL_DISCOVER_CHARACTERISTICS,
  CL_ENABLE_INDICATION,

  RUNNING
 }discovery_state_t;


 typedef struct button_state{
   bool button0_state;
   bool button1_state;
 } button_state_t;
 void discovery_state_machine(sl_bt_msg_t *evt);


typedef enum pbstate_s{

  no_event,

  procedure_completed,
  connection_open,
  connection_close,
  pb0_pressed,
  pb1_pressed,
}pbstate_t;

void pb0_pressed_external_signal();
void pb1_pressed_external_signal();
 void button_state(sl_bt_msg_t *evt);
#endif /* SRC_CLIENT_STATE_MACHINE_H_ */
