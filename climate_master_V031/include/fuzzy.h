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

#ifndef FUZZY_H_INCLUDED
#define FUZZY_H_INCLUDED


#define TASK_STAC_SIZE 128

#define TASK_CMD "ac"

#define TEMP_LIMIT_1     36     /**  Celsius degree ** 1. sensor temperetur limit.  The maximum temeprature**/
#define TEMP_LIMIT_2     36     /**  Celsius degree ** 2. sensor temperetur limit.  The maximum temeprature**/
#define TEMP_LIMIT_3     36     /**  Celsius degree ** 3. sensor temperetur limit.  The maximum temeprature**/

/** Regulation window, this parameter define what is the first temperatur when the fun start to working.**/

#define DELTA_1         10      /** 1. sensor **/
#define DELTA_2         10      /** 1. sensor **/
#define DELAT_3         10      /** 1. sensor **/

#define currentLimitDeviceID                101         /** the device ID wher I will mod the currnet **/
#define currentLimitUserRef                 1107        /** the device user refernece parmater number DC currnet limit 1138 **/

/** Get tempareture from the sensor "s" the sensor number **/
int getAktTemp(int s);

int sensNumb(void);

/** Inita and start working teh regualtion **/
void fuzzyInit(void);

/**Direct FAN OFF**/
void FAN_OFF(void);

/**Direct FAN ON full speed**/
void FAN_FULL(void);

/**Direct FAN speed % (0 = 0% 255 = 100% ) **/
void FAN_PER(int per);

/** Task function **/
void cmd_fuzzy(BaseSequentialStream *chp, int argc, char *argv[]);

/** return allert level **/
int allertlevel(void);

/** return hitemp level **/
int hitemplevel(void);

/** return origin AC churgng current level **/
int oro(void);

#endif // TASK1_H_INCLUDED
