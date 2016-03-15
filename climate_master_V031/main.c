/*

    Climate Master - Firfely Clean Energy UK  Balint Buchert
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "fuzzy.h"
#include "studer_can.h"
#include <button.h>
#include "climate_conf.h"
#include "comterm.h"
//#include "test.h"  // optinoal off
//#include "chprintf.h"
//#include "shell.h"
//#include "usbterm.h"
//#include "usbcfg.h"
//#include "usbconf.h"
//#include "evtimer.h"
//#include "string.h"
//#include "stdio.h"


/*===========================================================================*/
/* Command line related.                                                      */
/*===========================================================================*/

#define TEST_WA_SIZE    THD_WORKING_AREA_SIZE(256)

/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/**  life blinking hello word task **/
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {
  (void)arg;

  chRegSetThreadName("ON");

    while(true){
        pwmEnableChannel(&PWMD4, 2, 255);
        palSetPad(GPIOE, 8);
        pwmEnableChannel(&PWMD1, 0, 600);

        chThdSleep(1000);

        pwmEnableChannel(&PWMD4, 2, 0);

        pwmEnableChannel(&PWMD1, 0, 0);
        palClearPad(GPIOE, 8);

        chThdSleep(1000);
    }
}


static THD_WORKING_AREA(waThread2, 128);
static msg_t Thread2(void *arg ) {

  return 0;
}

/*
 * Application entry point.
 */

int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  /** halconf init**/
    halInit();

  /** task opeartiona init**/
    chSysInit();

  /** base port function init**/
    baseInit();

  /** Shell manager initialization. (USB terminal)*/
    shellInit();

  /** USB terminal Shell function  init**/
    usbTermInit();

  /** temperature sensor init */
    DS18B20_Init();

  /** Studer CAN init**/
    studerCAN_Init();

  /** Fuzzy regulation task start */
    fuzzyInit();

  /** SWITHING MODUL MODE SELECTION TASK*/
    buttonInit();

  /** Wifi 3G communication */
    wconfInit();

  /** Creates the hello word ledblinking thread.*/
    chThdCreateStatic(waThread1, sizeof(waThread1),
                    NORMALPRIO + 10, Thread1, NULL);
 /** task 2 **/
    /*chThdCreateStatic(waThread2, sizeof(waThread2),
                    NORMALPRIO + 10, Thread2, NULL); */

  while (true) {
     chThdSleepMilliseconds(1000);//500);
  }
}


