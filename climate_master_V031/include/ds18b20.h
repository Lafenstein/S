#ifndef __DS18B20
#define __DS18B20

/*
 * Copyright (C)  2015 Balint Buchert
 * Maxim DS18B20 driver to Chibios to STM32F406VGT
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/** \file ds18b20.h
    \brief ds18b20.h
*/

#include "ch.h"
#include "hal.h"

#define DS18B20_CONVERT_T_CMD				0x44
#define DS18B20_WRITE_STRATCHPAD_CMD		0x4E
#define DS18B20_READ_STRATCHPAD_CMD			0xBE
#define DS18B20_COPY_STRATCHPAD_CMD			0x48
#define DS18B20_RECALL_E_CMD				0xB8
#define DS18B20_SEARCH_ROM_CMD				0xF0
#define DS18B20_READ_POWER_SUPPLY_CMD		0xB4
#define DS18B20_SKIPROM_CMD					0xCC
#define DS18B20_MATCHROM_CMD				0x55

#define MAX_SENSORS 						5


#define DS18B20_1_GPIO						GPIOE       /** 1. sensro GPIO PORT **/
#define DS18B20_1_DATAPIN					2           /** 1. sensro GPIO PAD **/

#define DS18B20_2_GPIO                      GPIOE       /** 2. sensro GPIO PORT **/
#define DS18B20_2_DATAPIN					3           /** 2. sensro GPIO PAD **/

#define DS18B20_3_GPIO                      GPIOE       /** 3. sensro GPIO PORT **/
#define DS18B20_3_DATAPIN					4           /** 3. sensro GPIO PAD **/


#define numberOfSensors                     3

/** Theramal sensr task init and start **/
void DS18B20_Init(void);

/**
Get ds18B20 ROM code (identification code)
that function is automaic request the snesor code if that was inactive the last cylce
if you change the sensor when the program is runing two cycles later the snesor workig agen
**/
void DS18B20_GetROM(uint8_t SensorNum, uint8_t *buffer);

/**get temperature value from sensor  SensorNum
 - 1 is the first sensor
 - 2 is the secend sensor
 - 3 is the ...

return value with float format **/
float DS18B20_GetTemp(uint8_t SensorNum);

/** bit test is a bitrate tester function 10101010 signal
you abel to test the bitrate thats 1us van bit with scope.
**/
void bit_test(void);



void addSensor_ID(int id);


/** mode 2 Disbaled **/
//uint8_t DS18B20_ScanBus(void);

/** disbaled **/
//void DS18B20_StartConversion(uint8_t SensorNum);

#endif
