/*
 * Copyright (C)  2015 Balint Buchert
 * balintbuchert@gmail.com
 * Firefly Clean Energy -- Climate Master Projcet
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * File function:
 * Firfly switching modul button sensing and mode selection modul .**
 *
 */

/** \file
    \brief
*/

#include <stdlib.h>
#include <string.h>

#include "mcuconf.h"
#include <ch.h>
#include <hal.h>
#include <button.h>


#if TASK_STACK_SIZE <128

uint16_t BCC_ID            = 1138;    // BATTERY CHARGE CURRNET USER REFELENCE
uint16_t DEVICE_ID_IN_BCC  = 100;     // devices ID multicast
int      SBMCCV            = 0;       // STOP_BUTTON_MODE_CHARGING_CURRENT_VALUE
int      TPMCCV            = 50;      //3_PHASE_MODE_CHARGING_CURRENRT_VALUE
int      OPMCCV            = 41;      //ONE_PHASE_MODE_CHARGING_CURRENT_VALUE
int      FCMCCV            = 5;       //FLOAT_CHAERGE_MODE_CAHREGING_CURRENT_VALUE


thread_t *tp_button = NULL;  /** button inturput task pointer **/

int button_ID = 0;           /** button ID global varibale**/
            //#error the task minimum size is 128!
/**
                int ia = atoi(u);
                f = (float ) ia;

                uint32_t data;
               data = *(unsigned*)&f;

**/

/** int to float converser will go main thd**/
uint32_t inttofloat(int num){

    float f = (float) num;
    uint32_t hexfloat;
    hexfloat = *(unsigned*)&f;

return hexfloat;
}

static void led5off(void *arg) {

  (void)arg;
  palClearPad(GPIOD, GPIOD_LED6);
}

static THD_WORKING_AREA(waThreadButton, 128);
static THD_FUNCTION(ThreadButton, arg) {
  (void)arg;
  bool bfs;
  bool bns;

  chRegSetThreadName("ON_ext");


    while(true){
    /** lock "pause" interupt task **/
     chSysLock();
        tp_button = chThdGetSelfX();
        chSchGoSleepS(CH_STATE_SUSPENDED);
     chSysUnlock();

     extStop(&EXTD1);  /**disable EXT driver **/


    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 255);
     chThdSleep(5000);
    pwmEnableChannel(&PWMD4, 1, 0);
    chThdSleep(5000);

    /** READ button pin**/
    bns = palReadPad(GPIOC,button_ID);

    if ( !bns){
        switch(button_ID){
            case 0:
             CanWriteDATA( DEVICE_ID_IN_BCC , BCC_ID ,inttofloat(TPMCCV));
             //CanWriteDATA( 101 ,1202 ,inttofloat(8));
            break;

            case 1:
             CanWriteDATA( DEVICE_ID_IN_BCC ,BCC_ID ,inttofloat(FCMCCV));
             //CanWriteDATA( 101 ,1202 ,inttofloat(4));
            break;

            case 2:
             CanWriteDATA( DEVICE_ID_IN_BCC ,BCC_ID ,inttofloat(OPMCCV));
             //CanWriteDATA( 101 ,1202 ,inttofloat(2));
            break;

            case 3:
             CanWriteDATA( DEVICE_ID_IN_BCC ,BCC_ID ,inttofloat(SBMCCV));
             //CanWriteDATA( 101 ,1202 ,inttofloat(1));
            break;
        }
     }
    StartEXT(); /** EXT driver On agen **/
    }
}

/** button interupt universal **/
void buttonInterupt(){
    static virtual_timer_t vt4;

    chSysLock();
        thread_t *ntp = tp_button;
        if(ntp){
            tp_button = NULL;
            chSchWakeupS(ntp, MSG_OK);
        }
        chSysUnlock();

  palSetPad(GPIOD, GPIOD_LED6);

  chSysLockFromISR();
  chVTResetI(&vt4);

  /* LED4 set to OFF after 200mS.*/
  chVTSetI(&vt4, MS2ST(200), led5off, NULL);
  chSysUnlockFromISR();
}

/* Triggered when the button is pressed or released. The PC0 pin ( cable blue 3P mode).*/
static void extcb0(EXTDriver *extp, expchannel_t channel) {
  button_ID = 0;
  buttonInterupt();

}

/* Triggered when the button is pressed or released. The PC1 pin STOP MODE BUTTON */
static void extcb1(EXTDriver *extp, expchannel_t channel) {
  button_ID = 1;
  buttonInterupt();
}

/* Triggered when the button is pressed or released. The PC2 pin  BUTTON mode */
static void extcb2(EXTDriver *extp, expchannel_t channel) {
  button_ID = 2;
  buttonInterupt();
}

/* Triggered when the button is pressed or released. The PC3 pin  is set to ON.*/
static void extcb3(EXTDriver *extp, expchannel_t channel) {
  button_ID = 3;
  buttonInterupt();
}

static const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb0}, // PC0 STOP
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb1}, // PC1 MODE3
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb2}, // PC2 MODE2
    {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb3}, // PC3 MODE1
    //{EXT_CH_MODE_DISABLED, NULL},//
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};

void buttonInit(){

    extStart(&EXTD1, &extcfg);
    chThdCreateStatic(waThreadButton, sizeof(waThreadButton),
                    NORMALPRIO + 10, ThreadButton, NULL);
}

void StartEXT(){
extStart(&EXTD1, &extcfg);
}

#endif // TASK_STACK_SIZE

//  end of task
