
/*
 * Copyright (C)  2015 Balint Buchert
 * balintbuchert@gmail.com
 *
 * Firefly Clean Energy -- Climate Master Projcet
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/** \file usbterm
    \brief usbtren
*/

#include <stdlib.h>
#include <string.h>

#include <mcuconf.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>

#include "usbterm.h"
#include "studer_can.h"
#include "shell.h"
#include "usbcfg.h"

#if usbterm_STACK_SIZE < 128
#error Minimum task stack size is 128!
#endif

#if usbterm_SLEEP_TIME_US < 1
#error task sleep time must be at least 1
#endif

#if usbterm_SLEEP_TIME_US < 100
#warning task sleep time seems to be to few!
#endif

#define RELAY_PORT      GPIOE
#define RELAY_PIN       7

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)

static THD_WORKING_AREA(waThreadusbterm, usbterm_STACK_SIZE);

/**

*/
float f = 1;                                // temoprary float
uint32_t DEVICE_ID = 101 ;
uint16_t id;                // can parameter id varaibale

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

static void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%08lx %08lx %4lu %4lu %9s\r\n",
             (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
             (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
             states[tp->p_state]);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}


static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
  thread_t *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  if (tp == NULL) {
    chprintf(chp, "out of memory\r\n");
    return;
  }
  chThdWait(tp);
}


static void cmd_temp(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "temp\r\n");
    return;
  }
    while(true){
               chprintf(chp, "SENSOR_1: %d\r\n",getAktTemp(1));
               chprintf(chp, "SENSOR_2: %d\r\n",getAktTemp(2));
               chprintf(chp, "SENSOR_3: %d\r\n",getAktTemp(3));
               chprintf(chp,"alarm: %d\r\n", allertlevel());
               chprintf(chp,"hightemp: %d\r\n\n", hitemplevel());
               chprintf(chp,"%d\r\n\n", oro());

               chThdSleep(10000);
               return f;
    }
}


static void cmd_read(BaseSequentialStream *chp, int argc, char *argv[]){
(void)argv;
  if (argc > 0) {
    chprintf(chp, "temp\r\n");

    return;
  }
  int i =0;
  uint32_t rxd;
  uint32_t rxd2;
   uint32_t rxidd;
   DEVICE_ID = 101;
   CANRxFrameClone tempStack;
   //\id = 1107;

    while(i <=1 ){

               tempStack = CanReadDATA(DEVICE_ID,id);
               rxd     = ( tempStack.data8[4]<< 16 ) | ( tempStack.data8[5]<< 8 ) | ( tempStack.data8[6]);// | (rxmsg.data8[5]<< 8 ) | (rxmsg.data8[6] );
               rxd2    = ( tempStack.data8[7]<< 24 ) | ( tempStack.data8[8]<< 16 ) | ( tempStack.data8[9])<<8 | ( tempStack.data8[10] );
               rxidd   = ( tempStack.data8[3]<< 24 ) | ( tempStack.data8[2]<< 16 ) | ( tempStack.data8[1])<<8 | ( tempStack.data8[0] );

                i++;
    }
    i=0;
     chprintf(chp, "INT: %d\n\n\r", CanReadInt(DEVICE_ID,id));
     chprintf(chp, "RX_CAN: %X %X %X\n\n\r",rxidd ,rxd,rxd2);
    return f;
}

static void cmd_pid(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint8_t u[5];
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "id\r\n");
    return;
  }
    while(true){
               chprintf(chp,"Pleas ID:\r\n");

               chSequentialStreamRead(serial,&u,6);
               //chprintf(chp, "ID:--%s--\r\n",u);
               id = atoi(u);
               chprintf(chp, "ID_int:--%d--\r\n",id);

               chThdSleep(10000);
               return 1;
    }
}

static void cmd_pad(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint8_t u[4];
  //uint16_t id;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "id\n\r");
    return;
  }
    while(true){
               chprintf(chp, "Pleas DATA:\n\r");

               chSequentialStreamRead(serial,&u,5);
                int ia = atoi(u);
                f = (float ) ia;
                uint32_t data;
               //chprintf(serial, "int: %d\r\n",ia);
               //STdata = *(unsigned*)&f;
               data = *(unsigned*)&f;
               chprintf(serial, "value: %x\r\n",data);
               //chprintf(chp, "char:-%s-\n\n\r",u);
               CanWriteDATA(DEVICE_ID,id,data);
               chThdSleep(1000);
               return 1;
    }
}

static void cmd_dev(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint8_t u[3];
  (void)argv;
  if (argc > 0) {
    //chprintf(chp, "id\r\n\r");
    return;
  }
    while(true){

               chprintf(chp, "Pleas DEVICE_ID:\r\n");
               chSequentialStreamRead(serial,&u,4);
               DEVICE_ID = atoi(u);
               id = atoi(u);
                chprintf(chp, "DEV_ID_int: --%d--\r\n",id);
    return 1;
    }
}

static void cmd_relay(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint8_t u[3];
  (void)argv;
  if (argc > 0) {
    chprintf(chp, "id\r\n\r");
    return;
  }
    while(true){
               //palSetPad(RELAY_PORT, RELAY_PIN);
               chprintf(chp, "RELAY STATES 0 V 1:\n\r",getAktTemp(1));

               chSequentialStreamRead(serial,&u,4);
               chprintf(chp, "STATE: --%s--\n\n\r",u);

                if(atoi(u)){
                    palSetPad(RELAY_PORT, RELAY_PIN);
                    chprintf(chp, "RELAY ON\n\r");
                }
                else{
                    palClearPad(RELAY_PORT, RELAY_PIN);
                    chprintf(chp, "RELAY OFF\n\r");
                    }

               return 1;
    }
}


static void cmd_wifi(BaseSequentialStream *chp, int argc, char *argv[]) {

  uint8_t *u;
  (void)argv;

    while(true){
               //palSetPad(RELAY_PORT, RELAY_PIN);
               chprintf(chp, "wifi: Processing...\n\r");
               //chSequentialStreamRead(serial,&u,3);
                wifiInit();
            return 1;
    }
}


static void cmd_telreconf(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;

    chprintf(chp, "reconfig...\n\r");
    telnetReconfig();
    chprintf(chp, "done:\n\r");
    return 1;
}

static void cmd_tx(BaseSequentialStream *chp, int argc, char *argv[]) {
  (void)argv;

   uint8_t crack[32];
   int i =1;
   while(i == 1){
                   uint8_t b = pubA();
                   chprintf(chp, "%c",b);

                    pubH();
                    chprintf(chp, "%c",*crack);



                 if(b == '!'){
                    chprintf(chp, "closed \n\r",b);
                    return 1;
                    }
                  else{
                 //chprintf(chp, "%c",*crack);
                 chThdSleep(100);
                }
    }
     return 1;
}

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {"t", cmd_temp},
  {"pid", cmd_pid},
  {"pda", cmd_pad},
  {"dev", cmd_dev},
  {"relay", cmd_relay},
  {"r", cmd_read},
  {"w", cmd_wifi},
  {"n", cmd_telreconf},
  {"x", cmd_tx},
  {NULL, NULL}
};

static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SDU1,
  commands
};



__attribute__((noreturn))
static THD_FUNCTION(Threadusbterm, arg) {
    (void) arg;
    chRegSetThreadName("usbterm");

    thread_t *shelltp = NULL;

     /** Initializes a serial-over-USB CDC driver.*/
    sduObjectInit(&SDU1);
    sduStart(&SDU1, &serusbcfg);

    /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */

    usbDisconnectBus(serusbcfg.usbp);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);

    /** a shell respawn upon its termination. */
    while(TRUE) {

        //sdWrite(&SD3, (uint8_t *) data, strlen(data));
    if (!shelltp) {
    //shelltp = shellCreate(&shell_cfg2, SHELL_WA_SIZE, NORMALPRIO);
      if (SDU1.config->usbp->state == USB_ACTIVE) {
        /* Spawns a new shell.*/
        shelltp = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
      }
    }
    else {
      /* If the previous shell exited.*/
     if (chThdTerminatedX(shelltp)) {
        /* Recovers memory of the previous shell.*/
        chThdRelease(shelltp);
        shelltp = NULL;
      }
    }
     chThdSleepMilliseconds(1000);//500);
       //chThdSleepMicroseconds(usbterm_SLEEP_TIME_US);

    }
    //chThdExit(1);
}

/**
    usbterm user interface
*/
void cmd_usbterm(BaseSequentialStream *chp, int argc, char *argv[]) {
    (void) argc;
    (void) argv;
    (void) chp;
}
void usbTermInit(void)
{

  chThdCreateStatic(waThreadusbterm, sizeof(waThreadusbterm), NORMALPRIO, Threadusbterm, NULL);
}
