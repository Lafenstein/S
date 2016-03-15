#ifndef STUDER_CAN_H_INCLUDED
#define STUDER_CAN_H_INCLUDED


#define TASK_STAC_SIZE 128
/*
 * Copyright (C)  2015 Balint Buchert balintbuchert@gmail.com
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

// a task össze paramétert amit más is láthat....

#define TASK_CMD "ac"

//uint32_t texmex[3];


/**
* @brief   CAN received frame.
* @note    Accessing the frame data as word16 or word32 is not portable because
 *          machine data endianness, it can be still useful for a quick filling.
 */
 typedef struct {
  struct {
     uint8_t                 DLC:4;          /**< @brief Data length.        */
     uint8_t                 RTR:1;          /**< @brief Frame type.         */
     uint8_t                 IDE:1;          /**< @brief Identifier type.    */
   };
   union {
     struct {
       uint32_t              SID:11;         /**< @brief Standard identifier.*/
     };
     struct {
       uint32_t              EID:29;         /**< @brief Extended identifier.*/
     };
   };
   union {
     uint8_t                 data8[12];       /**< @brief Frame data.         */
     uint16_t                data16[4];      /**< @brief Frame data.         */
     uint32_t                data32[2];      /**< @brief Frame data.         */
   };
 } CANRxFrameClone;


//#define
void studerCAN_Init(void);

uint8_t studerCAN_START(void);

void cmd_studerCAN(BaseSequentialStream *chp, int argc, char *argv[]);

void CanWriteDATA(uint32_t TO, uint32_t PID,  uint32_t PDATA );

int CanReadInt(uint16_t FROM, uint16_t PID);

CANRxFrameClone CanReadDATA(uint16_t FROM, uint16_t PID);

// IEEE745 hex to float converter
uint32_t floatToIEEE745( float value );

// IEEE745 float to hex converter
float IEEE745toFloat( uint32_t value);



#endif // STUDER_CAN_H_INCLUDED
