/*
 * Copyright (C)  2015 Balint Buchert
 * balintbuchert@gmail.com
 * Studer invert publick CAN bus driver.**
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/** \file
    \brief
*/

#include <stdlib.h>
#include <string.h>

#include <mcuconf.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <studer_can.h>

#if TASK_STACK_SIZE <128

            //#error the task minimum size is 128!
#endif // TASK_STACK_SIZE

SerialUSBDriver SDU3;

static THD_WORKING_AREA(waTh_studer_can, TASK_STAC_SIZE);

uint32_t volatile STdata_S;  // can data varibale
uint32_t rxdata = 7;         // CAN receives data
uint32_t rxdata2 = 7;         // CAN receives data
uint32_t rxid = 7;         // CAN receives data
uint16_t id_S;                // can parameter id varaibale

uint32_t DEVICE_ID_S = 100 ; // defoult value multicasting addres.
uint32_t SERVICES_ID_S = 0x00;//    Flag 6bit 0x00 // deafult read mode
int DATA_LENGHT_RW_S = 4;  // deafult data lenght 2 read mode

CANRxFrameClone texmex;   // temproary can rx structure

thread_t *tp_S = NULL;  // can send task pointer


/***********************************************************/

float IEEE745toFloat( uint32_t value){

    float retek;
        retek = *(float*)&value;
        return retek;
}

uint32_t floatToIEEE745( float value){

    uint32_t cseresznye;
        cseresznye = *(unsigned*)&value;
        return cseresznye;
}


struct can_instance {
  CANDriver     *canp;
  uint32_t      led;
};

/**
 * 500KBaud, automatic wakeup, automatic recover
 * from abort mode.
 * See section 22.7.7 on the STM32 reference manual.
 * speed = 42000000 / (BRP + 1) / (1 + TS1 + 1 + TS2 + 1)
 * 			42000000 / 7 / 12 = 500000
 **/

static const CANConfig cancfg = {
  /*.mcr = */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /*.btr = */ CAN_BTR_SJW(0) | CAN_BTR_TS2(1) // | CAN_BTR_LBKM
  | CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};

static const CANConfig cancfg2 = {
  /*.mcr = */ CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  /*.btr = */ CAN_BTR_SJW(0) | CAN_BTR_TS2(1)  | CAN_BTR_LBKM
  | CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};


void CanWriteDATA(uint32_t TO, uint32_t PID,  uint32_t PDATA ){

// config write mode
 SERVICES_ID_S = 0x2;
 DATA_LENGHT_RW_S = 7;

 DEVICE_ID_S = TO;
 id_S = PID;
 STdata_S = PDATA;

 // run send can
 chSysLock();
    thread_t *ntp = tp_S;
    if(ntp){
        tp_S = NULL;
        chSchWakeupS(ntp, MSG_OK);
    }
 chSysUnlock();
// auto stop

}

CANRxFrameClone CanReadDATA(uint16_t FROM, uint16_t PID){

 //uint32_t Rdata = 8; // idejglenes tarolo

 SERVICES_ID_S = 0x1; // serveice ID Read mode
 DATA_LENGHT_RW_S = 2; // data length in read mode

 DEVICE_ID_S = FROM;  // from device
 id_S = PID;          // form parameter ID

// write data reqest
  chSysLock();
    thread_t *ntp = tp_S;
    if(ntp){
        tp_S = NULL;
        chSchWakeupS(ntp, MSG_OK);
    }
 chSysUnlock();

  chThdSleep(4000);
return texmex;

// start can reading
// ellenorzes

}
int CanReadInt(uint16_t FROM, uint16_t PID){

CANRxFrameClone temprx;
uint32_t temp32 = 0;
float    tempf = 0;
int      tempi = 0;

 temprx = CanReadDATA(FROM, PID);
 temp32 = ( temprx.data8[7]<< 24 ) | ( temprx.data8[8]<< 16 ) | ( temprx.data8[9])<<8 | ( temprx.data8[10] );
 tempf = *(float*)&temp32;
 tempi = tempf;

return tempi;
}

static THD_WORKING_AREA(waThCanSend, 256);
static THD_FUNCTION(ThCanSend, arg) {
CANTxFrame txmsg;

  (void)arg;
  chRegSetThreadName("transmitter");
  uint8_t nibble = 0;
  //uint32_t STdata2= 0x40000000;// = adat();

  uint32_t ST_ID = 0x00000000; // blank id
  uint32_t XTM_ID = 101;//     DST_ID  10bit
  uint32_t MY_ID = 0x0BB;//      SRC_ID  10bit
  uint32_t SERVICE = 0x1;//    Service 3bit-- 0 user info read -- 1 read --- 2 write
  uint32_t FLAG = 0x00;//    Flag 6bit 0x00

  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = ST_ID;        /**write to 101 flrom 0BB**/
  txmsg.RTR = CAN_RTR_DATA;

  while (true){



    chThdSleepMilliseconds(500);//?

            txmsg.data8[0] = id_S >> 8;  /**chop the 2HEX to 2 1Hex **/
            txmsg.data8[1] = id_S >> 0;

            txmsg.data8[3] = STdata_S >> 24;    /** data shift 32bit ot 4 section **/
            txmsg.data8[4] = STdata_S >> 16;
            txmsg.data8[5] = STdata_S >> 8;
            txmsg.data8[6] = STdata_S >> 0;

            ST_ID = (DEVICE_ID_S << 19) | (MY_ID << 9) | (SERVICES_ID_S << 6) | FLAG; /**Studer iD  creation **/

            txmsg.EID = ST_ID;                                                        /** Studer ID add tha CAN ID Structure**/
            txmsg.DLC = DATA_LENGHT_RW_S;                                             /** Data Lenght add the CAN Data lenght**/

            //canTransmit(&CAND2, 1, &txmsg, 100);                                      /** CAN package sendt out CAN2 port **/
            canTransmit(&CAND1, 1, &txmsg, 100);                                      /** CAN package sendt out CAN1 port **/


    chSysLock();
    tp_S = chThdGetSelfX();
    chSchGoSleepS(CH_STATE_SUSPENDED);
    chSysUnlock();
            //canTransmit(&CAND2, 1, &txmsg, MS2ST(100));

  }

}

/*
 * Receiver thread.
 */

static THD_WORKING_AREA(can_rx1_wa, 256);
static THD_FUNCTION(can_rx, p) {
  struct can_instance *cip = p;
  event_listener_t el;
  CANRxFrameClone rxmsg;
  rxmsg.DLC = 7;


  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&cip->canp->rxfull_event, &el, 0);

  while(!chThdShouldTerminateX()){//(true){
  canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE);
  //canReceive(&CAND1, NULL , &rxmsg, TIME_IMMEDIATE);
    chThdSleepMilliseconds(100);
  texmex = rxmsg;
  chThdSleepMilliseconds(100);
  texmex = rxmsg;

  //rxdata = (rxmsg.data8[3]<< 24 )| (rxmsg.data8[4]<< 16 );// | (rxmsg.data8[5]<< 8 ) | (rxmsg.data8[6] );
  //rxdata2 = (rxmsg.data8[0]<< 8 ) | (rxmsg.data8[1] );
  //rxid = rxmsg.EID;
  //rexd[0] = rxmsg.EID;
  //rxdata = rxmsg.data8[2] << 16;
  //chprintf(&SDU3, "%X\r\n",rxdata);
  //rxdata = 0x12345678;
  //chThdSleepMilliseconds(100);

  }
}



static THD_FUNCTION(Th_studer_can, arg) {
  systime_t time;                   /* Next deadline.*/

  (void)arg;
  chRegSetThreadName("studer_can");
    while (true) {
  chThdCreateStatic(waTh_studer_can, sizeof(waTh_studer_can), NORMALPRIO+10, Th_studer_can, NULL);
    }

}
// init  studer CAN bus  task
void studerCAN_Init(){


/**CAN SEND TASK*/

  // Activates CAN driver.
  canStart(&CAND1, &cancfg);
  canStart(&CAND2, &cancfg2);


   palSetPadMode(GPIOD, 0, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);
   palSetPadMode(GPIOD, 1, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);

   //palSetPadMode(GPIOB, 13, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);
   //palSetPadMode(GPIOB, 12, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);
   palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);
   palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(9) | PAL_STM32_OSPEED_HIGHEST /*|PAL_STM32_OTYPE_OPENDRAIN*/);

  // enable CAN2: PB5 -> alt1 = CAN2_RX, PB6 -> alt2 = CAN2_TX|
  //GPIOB->AFRL = (GPIOB->AFRL & ~(0xFF << 4*5)) | (0x99 << 4*5);
  // set PB5 and PB6 to alternate function mode
  //GPIOB->MODER = (GPIOB->MODER & ~(0b1111 << 2*5)) | (0b1010 << 2*5);

  /**Can send task start **/
   chThdCreateStatic(waThCanSend, sizeof(waThCanSend), NORMALPRIO + 3,
                    ThCanSend, NULL);
  /**Can receve task start **/
   chThdCreateStatic(can_rx1_wa, sizeof(can_rx1_wa), NORMALPRIO + 7,
                   can_rx, NULL);


    //chThdCreateStatic(waTh_studer_can, sizeof(waTh_studer_can),
      //              NORMALPRIO + 10, Th_studer_can, NULL);

}

//  end of task
