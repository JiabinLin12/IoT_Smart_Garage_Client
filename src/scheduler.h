/***********************************************************************
 *@file        scheduler.c
 *
 *@version     0.0.1
 *
 *@brief       scheduler header files
 *
 *@author      Jiabin Lin, jili9036@Colorado.edu
 *
 *@date        Nov 13th, 2021
 *
 *@institution University of Colorado Boulder (UCB)
 *
 *@course      ECEN 5823-001: IoT Embedded Firmware (Fall 2021)
 *
 *@instructor  David Sluiter
 *
 *@assignment  final-project
 *
 *@due         Dec 3rd, 2020
 *
 *@resources   Utilized Silicon Labs' EMLIB peripheral libraries to
 *             implement functionality.
 *
 *
 *@copyright   All rights reserved. Distribution allowed only for the
 *             use of assignment grading. Use of code excerpts allowed at the
 *             discretion of author. Contact for permission.  */
#ifndef SRC_SCHEDULER_H_
#define SRC_SCHEDULER_H_

#include "app.h"
#include "src/gpio.h"


#define CIR_QUEUE_SIZE      16



enum {
  no_event                  = 0,
  letimer_comp1_expired     = 1,

  pb0_pressed = 2,

  procedure_completed=3,
  connection_open = 4,
  connection_close = 5,

};




typedef struct QueueElement_s{
  uint16_t charHandle;
  size_t bufferLength;
  uint8_t buffer[5];
  uint16_t charactristic;
  bool invalid_element;
}QueueElement_t;

//structure for event circular queue
typedef struct CirQueue_s{
  int32_t readptr;
  int32_t writeptr;
  bool isfull;

  QueueElement_t elements[CIR_QUEUE_SIZE];
}CirQueue_t;

//Function prototype
CirQueue_t *getPbQ();
void PbCirQ_init();
void pushbutton0_response();
void pb_pressed_external_signal();
//void schedulerSetEventReadTemp();
//void update_and_send_indication();
void schedulerSetEventWaitUs();
//void schedulerSetEventI2Cdone();
void light_to_client_indication(uint8_t light_state);
void motion_to_client_indication(uint8_t motion_state);
//uint32_t getNextEvent();
void soft_timer_deq_indication();
#endif /* SRC_SCHEDULER_H_ */
