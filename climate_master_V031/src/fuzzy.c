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
/** \file fuzzy
    \brief fuzzy
*/

#include <stdlib.h>
#include <string.h>

#include <mcuconf.h>
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <ds18b20.h>
#include <studer_can.h>

#include "fuzzy.h"

#define TASK_STAC_SIZE2   THD_WORKING_AREA_SIZE(256)
//#define TTASK_STACK_SIZE    THD_WORKING_AREA_SIZE(256)
#if TASK_STACK_SIZE <128

#define FAN_CH                  0       // pwm fan output chanel
            //#error the task minimum size is 128!
#endif // TASK_STACK_SIZE


    int AktTemp2 = 220; // infunction

    float TEMP[numberOfSensors];       /** perefert temperature  array, dinamic variables. **/
    int DELTA[numberOfSensors];        /** temperature fuzzy delta " window "C **/
    int SensorPry[numberOfSensors];     /** temepreture sensor priroity **/
                                    /** numberOfSensors --> ds18B20.h **/
    bool OffState = 1;
    int StartLimitSpeed = 100;
    int StartPWM = 200;
    int FanSpeed  = 0;

    volatile int currentTemp2[3];
    volatile float lastReadings2[12];

    int alert = 0;
    bool quit = 0;
    int errorLevel = 0;
    int delayT = 100;
    int originCurrent = 1;
    bool hiteup = 0;


    static THD_WORKING_AREA(waThfuzzy, TASK_STAC_SIZE);
    static THD_WORKING_AREA(waThCurrentLimiter, TASK_STAC_SIZE2);


void FAN_OFF(void){
    pwmEnableChannel(&PWMD4, FAN_CH, 0);
}

void FAN_FULL(void){
    pwmEnableChannel(&PWMD4, FAN_CH, 255);
}

void FAN_PER(int per){
    pwmEnableChannel(&PWMD4, FAN_CH, per);
}

void FAN_DRIVE(int d){
    pwmEnableChannel(&PWMD4, FAN_CH, d);
}

/**FAN Soft start on low speed **/
void FAN_SOFT_START(void){
    FAN_PER(StartPWM);
    chThdSleepMilliseconds(500);  // run up time
}

void getAllTemps(){

        currentTemp2[0] = DS18B20_GetTemp(1);
        chThdSleep(500);
        currentTemp2[1] = DS18B20_GetTemp(2);
        chThdSleep(500);
        currentTemp2[2] = DS18B20_GetTemp(3);
        chThdSleep(500);
}

void fuzzy_variables_inint(void){

        TEMP[0] = TEMP_LIMIT_1;
        TEMP[1] = TEMP_LIMIT_2;
        TEMP[2] = TEMP_LIMIT_3;

        DELTA[0] = DELTA_1;
        DELTA[1] = DELTA_2;
        DELTA[2] = DELAT_3;
}

void sendAlert(int a){

    switch(a){
        case 1 :{
            CanWriteDATA(currentLimitDeviceID, currentLimitUserRef,floatToIEEE745(originCurrent));
            }
            break;

        case 2 :{
            CanWriteDATA(currentLimitDeviceID, currentLimitUserRef,floatToIEEE745(originCurrent*0.8));
            }
            break;

        case 3:{
            CanWriteDATA(currentLimitDeviceID, currentLimitUserRef,floatToIEEE745(originCurrent*0.6));
            }
            break;

        case 4:{
            CanWriteDATA(currentLimitDeviceID, currentLimitUserRef,floatToIEEE745(originCurrent*0.5));
            }
            break;
    }
}

static THD_FUNCTION(ThCurrentLimiter, arg) {
  //systime_t time;                   /* Next deadline.*/
  (void)arg;
  chRegSetThreadName("TempCurrenrLimiter");
    // read churging current
    int t = 0;
    int j= 0;
    int i = 0;
    bool fn = 0;

    originCurrent = CanReadInt(currentLimitDeviceID, currentLimitUserRef);

    while( true ){
        if( alert == 0){
            while(i <= 2 && !quit){
                if( currentTemp2[i] >= TEMP[i]){
                    alert = 1;
                    quit = 1;
                }
                i++;
            }
        i = 0;
      }
       else{
           if(alert >> 1 ){
            sendAlert(alert);
            fn = 1;
            }
        }
        while(t <= delayT){
            t++;
            while( j <=2 && currentTemp2[j] <= TEMP[j]){
                j++;
            }
            if(j <= 2 && !hiteup ){
                hiteup= 1;
                t = 0;
            }
            else{
                if(hiteup && j >= 3){
                    hiteup = 0;
                    t = 0;
                }
            }
        //sleep task 10s
        j = 0;
        chThdSleep(1000);
        }
        if(hiteup){
            if(4 >> alert){
              ++alert;
              }
              else{
              alert = 4;
              }
        }
        else{
            if(alert >> 0){
            alert--;}
            else{
                if(fn){
                sendAlert(1);
                fn = 0;
                }
            }
        }
      //  i = 0;
        quit = 0;
        t = 0;
        chThdSleep(100);
    }
}

int allertlevel(void){
    return alert;
}

int hitemplevel(void){
    return hiteup;
}

int oro(void){
    return originCurrent;
}

static THD_FUNCTION(Thfuzzy, arg) {
  systime_t time;                   /* Next deadline.*/
  (void)arg;
    chRegSetThreadName("fuzzy");
    int per = 0;

    while (true) {

        getAllTemps();
        int s = 0;
        for(s ; s != 3; s++){
            if(currentTemp2[s] < TEMP[s] - DELTA[s]){
            /** OFF **/

            SensorPry[s] = 0;
            }
        else
            if (currentTemp2[s] > TEMP[s] - DELTA[s] && currentTemp2[s] < TEMP[s]){

            /** Fan  PER = 1 - (TEMP - AktTemp) /DELAT  == % /100 -->> full is 255 (PWM)
            Fan PER = ( 1- ((TEMP - AktTemp)/ DELAT)) * 255  **/

            per = ((1-((TEMP[s] - currentTemp2[s])/ DELTA[s])) * 255) ;
            SensorPry[s] = per-20;
            }
            else{
                if(currentTemp2[s] >= TEMP[s]){
                 /**Full ON**/

                SensorPry[s] = 255;
                }
            }
        /** FAN Low speed starter **/

         int newSpeed = max_p();

         if(newSpeed != FanSpeed && newSpeed > 30 )
            if(FanSpeed == 0){
                FAN_SOFT_START();
                FanSpeed = newSpeed;
                FAN_DRIVE(FanSpeed);
                }
            else{
               FanSpeed = newSpeed;
               FAN_DRIVE(FanSpeed);
               }
         else
           if(FanSpeed != 0 && newSpeed < 10){
            FanSpeed = 0;
            FAN_DRIVE(FanSpeed);
            }
        }
    } // end of while
  chThdSleep(10000);
  chThdCreateStatic(waThfuzzy, sizeof(waThfuzzy), NORMALPRIO+10, Thfuzzy, NULL);

}

int max_p(){
    int i= 0;
    int BP = 0;
    BP = SensorPry[i];
    for(i = 1 ; i != 3 ; i ++){
        if(SensorPry[i]> BP)
            BP = SensorPry[i];
    }
return BP;
}

// start fuzzy regulation task
void fuzzyInit(void){
    fuzzy_variables_inint();

    chThdCreateStatic(waThfuzzy, sizeof(waThfuzzy),
                    NORMALPRIO + 10, Thfuzzy, NULL);

    originCurrent = CanReadInt(currentLimitDeviceID, currentLimitUserRef);

    chThdSleep(10000);


    chThdCreateStatic(waThCurrentLimiter, sizeof(waThCurrentLimiter),
                   NORMALPRIO + 10, ThCurrentLimiter, NULL);
}

// retun back sensor temp value int.
int getAktTemp(int s) {
return currentTemp2[s-1];

}

int sensNumb(){
    int a = DS18B20_ScanBus();
return a;
}
//  end of task
