/**
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
 *
 *
 * File function:
 *  DS18B20 1 wire Thermal sensor Driver to STM32F4
 *   Automatic ROM code searcher
 *   separet 3 sensor pin outs --> ds18b20.h
 *
 */
/** \file  ds18B20.c
    \brief ds18B20.c
*/



#include "ds18b20.h"

uint8_t ROM_ADRESS_1[8]; // dinamic rom address  PIN 1
uint8_t ROM_ADRESS_2[8]; // dinamic rom address  PIN 2
uint8_t ROM_ADRESS_3[8]; // dinamic rom address  PIN 3
uint8_t SEN_ID;  // sensor cal back ID -1 in switch cases 1 is the first

void OneWire_DataPinInput(int c)
{
switch(c-1){
    case 0 :
        palSetPadMode(DS18B20_1_GPIO, DS18B20_1_DATAPIN, PAL_MODE_INPUT);
    break;

    case 1 :
        palSetPadMode(DS18B20_2_GPIO, DS18B20_2_DATAPIN, PAL_MODE_INPUT);
    break;

    case 2 :
        palSetPadMode(DS18B20_3_GPIO, DS18B20_3_DATAPIN, PAL_MODE_INPUT);
    break;

    }
}

void OneWire_DataPinOutput(int c)
{
    switch(c-1){
    case 0 :
        palSetPadMode(DS18B20_1_GPIO, DS18B20_1_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
    break;

    case 1 :
        palSetPadMode(DS18B20_2_GPIO, DS18B20_2_DATAPIN , PAL_MODE_OUTPUT_PUSHPULL);
    break;

    case 2 :
        palSetPadMode(DS18B20_3_GPIO, DS18B20_3_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
    break;

    }
}


void OneWire_SetBusLine(uint8_t state, uint8_t ch)
{
	if(state)
	{
        switch(ch-1){
            case 0 :{
                palSetPadMode(DS18B20_1_GPIO, DS18B20_1_DATAPIN, PAL_MODE_INPUT);
                palWritePad(DS18B20_1_GPIO, DS18B20_1_DATAPIN, 1);
            }
            break;

            case 1 :{
                palSetPadMode(DS18B20_2_GPIO, DS18B20_2_DATAPIN , PAL_MODE_INPUT);
                palWritePad(DS18B20_2_GPIO, DS18B20_2_DATAPIN, 1);
            }
            break;

            case 2 :{
                palSetPadMode(DS18B20_3_GPIO, DS18B20_3_DATAPIN, PAL_MODE_INPUT);
                palWritePad(DS18B20_3_GPIO, DS18B20_3_DATAPIN, 1);
            }
            break;
        }
	}
	else
	{
		switch(ch-1){
            case 0 :{
                palSetPadMode(DS18B20_1_GPIO, DS18B20_1_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
                palWritePad(DS18B20_1_GPIO, DS18B20_1_DATAPIN, 0);
            }
            break;

            case 1 :{
                palSetPadMode(DS18B20_2_GPIO, DS18B20_2_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
                palWritePad(DS18B20_2_GPIO, DS18B20_2_DATAPIN, 0);
            }
            break;

            case 2 :{
                palSetPadMode(DS18B20_3_GPIO, DS18B20_3_DATAPIN, PAL_MODE_OUTPUT_PUSHPULL);
                palWritePad(DS18B20_3_GPIO, DS18B20_3_DATAPIN, 0);
            }
            break;
        }
	}
}

uint8_t OneWire_GetBusLine(int c)
{
switch (c-1){
case 0 :
	return palReadPad(DS18B20_1_GPIO, DS18B20_1_DATAPIN);
    break;
case 1 :
    return palReadPad(DS18B20_2_GPIO, DS18B20_2_DATAPIN);
    break;
case 2:
    return palReadPad(DS18B20_3_GPIO, DS18B20_3_DATAPIN);
    break;
    }
}

static void Delay_us(uint32_t us)
{
    chSysLock();
    int r = 0;
    int u = 42;
    uint32_t t = u * us;
    for (r ; r< t; r++){
    asm("nop");
    }

    chSysUnlock();
}

void bit_test(void){
 while(true){
    OneWire_SetBusLine(0,SEN_ID);
	Delay_us(1);
	OneWire_SetBusLine(1,SEN_ID);

}
}


uint8_t OneWire_ReadBit(void)
{
	uint8_t bit = 0;

	OneWire_SetBusLine(0,SEN_ID);
	Delay_us(2);
	OneWire_SetBusLine(1,SEN_ID);
	Delay_us(15);
	if(OneWire_GetBusLine(SEN_ID))
		bit = 1;
	Delay_us(38);

	return bit;
}

void OneWire_WriteBit(uint8_t bit)
{
	OneWire_SetBusLine(0,SEN_ID);
	Delay_us(3);
	if(bit)
		OneWire_SetBusLine(1,SEN_ID);
	else
		OneWire_SetBusLine(0,SEN_ID);
	Delay_us(60);
	OneWire_DataPinInput(SEN_ID);
	OneWire_SetBusLine(1,SEN_ID);
}

void OneWire_WriteByte(uint8_t data)
{
	uint8_t _data = data;
    int i = 0;
	for(i; i<8; i++)
	{
		OneWire_WriteBit(_data & 0x01);
		_data>>= 1;
	}
	}

uint8_t OneWire_ReadByte(void)
{
	uint8_t data = 0;
    int i = 0;
	for( i; i<7; i++)
	{
		if(OneWire_ReadBit())
			data|= 0x80;
		data>>= 1;
	}
	if(OneWire_ReadBit())
		data|= 0x80;

	return data;
}

uint8_t OneWire_Reset( int RID)
{
	OneWire_SetBusLine(0,RID); // out
	Delay_us(480);

	OneWire_SetBusLine(1,RID); // read Presence

	Delay_us(60);
	if(!OneWire_GetBusLine(RID))
	{
		Delay_us(420);
		return 1;
	}
    else return 0;
}

void DS18B20_Init(void)
{
    OneWire_DataPinOutput(1);
    OneWire_DataPinOutput(2);
    OneWire_DataPinOutput(3);
	OneWire_Reset(1);
	OneWire_Reset(2);
	OneWire_Reset(3);
	ROM_CodeSearcher(1);
	ROM_CodeSearcher(2);
    ROM_CodeSearcher(3);

}

void writeRomCode(int d){
int i;
uint8_t *a = ROM_ADRESS_1; //{0x28, 0xC9, 0x30, 0x01, 0x07, 0x00, 0x00, 0x66 };
uint8_t *b = ROM_ADRESS_2;//{0x28, 0xC1, 0x29, 0x01, 0x07, 0x00, 0x00, 0x48 };
uint8_t *c = ROM_ADRESS_3;


    for(i = 0; i < 8; i++){
    switch (d) {
        case 1:
            OneWire_WriteByte(a[i]);
        break;

        case 2:
            OneWire_WriteByte(b[i]);
        break;

        case 3:
            OneWire_WriteByte(c[i]);
        break;
        }
    }
}

void setPad(int c){

    switch (c-1){
    case 0 :
        palSetPad(DS18B20_1_GPIO, DS18B20_1_DATAPIN);
    break;
    case 1 :
        palSetPad(DS18B20_2_GPIO, DS18B20_2_DATAPIN);
    break;
    case 2:
        palSetPad(DS18B20_3_GPIO, DS18B20_3_DATAPIN);
    break;
    }
}

void addSensor_ID(int id){
SEN_ID = id;

}

float DS18B20_GetTemp(uint8_t SensorNum)
{

	uint8_t TL, TH;
	uint16_t Temp = 220;
    int i = 0;
    SEN_ID = SensorNum;

            chSysLock();
            OneWire_Reset(SEN_ID);

            OneWire_WriteByte(0x55);

            writeRomCode(SensorNum); // sned ROM code //64 bit rom code

            OneWire_WriteByte(0x44); //

            OneWire_SetBusLine(0,SEN_ID); // setup lint to output

            setPad(SEN_ID); // parasita modepower to the line.

            chThdSleep(7500); // delay 750ms

            OneWire_Reset(SEN_ID);

            OneWire_WriteByte(0x55);
            //64 bit rom code
            writeRomCode(SensorNum);
            OneWire_WriteByte(0xBE);


            OneWire_SetBusLine(1,SEN_ID);


            TL = OneWire_ReadByte();
            TH = OneWire_ReadByte();

            chSysUnlock();
            if(TL==0 && TH==0) return 0;

            Temp = (int16_t)(TL)|((int16_t)TH<<8);
            Temp*= 0.0625;

            if(Temp > 250)
                ROM_CodeSearcher(SEN_ID);
            else

	return Temp;

}

/**read sensor rom code **/
void ROM_CodeSearcher(int id){
    switch(id-1){
    case 0:{
        int i;
        OneWire_Reset(SEN_ID);
        OneWire_WriteByte(0x33);
        OneWire_SetBusLine(1,1);
        for(i = 0 ; i < 8 ; i++){
        ROM_ADRESS_1[i] = OneWire_ReadByte();
        }
    }
    break;
    case 1:{
        int j;
        OneWire_Reset(SEN_ID);
        OneWire_WriteByte(0x33);
        OneWire_SetBusLine(1,2);
        for(j = 0 ; j < 8 ; j++){
        ROM_ADRESS_2[j] = OneWire_ReadByte();
        }
    }
    break;
    case 2:{
        int k;
        OneWire_Reset(SEN_ID);
        OneWire_WriteByte(0x33);
        OneWire_SetBusLine(1,3);
        for(k = 0 ; k < 8 ; k++){
        ROM_ADRESS_3[k] = OneWire_ReadByte();
        }
    }
    break;




    }
}

/*
void DS18B20_StartConversion(uint8_t SensorNum)
{

	OneWire_Reset();
	if(SensorNum==0)
	{
		OneWire_WriteByte(0xCC);
	}
	else
	{
		OneWire_WriteByte(DS18B20_MATCHROM_CMD);
		unsigned char i=0;
		for(i; i<8; i++)
		{
			OneWire_WriteByte(DS18B20_SERIAL_NUMBER[SensorNum-1][i]);
		}
	}
	OneWire_WriteByte(0x44);

}

uint8_t OneWire_SearchROM(uint8_t diff, uint8_t *id)
{
	uint8_t DataBit, DataByte, next_diff, bit;

	if(!OneWire_Reset())
		return 0xFF;

	OneWire_WriteByte(DS18B20_SEARCH_ROM_CMD);
	next_diff = 0x00;

	DataBit = 64;
	do
	{
		DataByte = 8;
		do
		{
			bit = OneWire_ReadBit();
			if(OneWire_ReadBit())
			{
				if(bit)
				return 0xFE;
			}
			else
			{
				if(!bit)
				{
					if(diff>DataBit || ((*id&1) && diff!=DataBit))
					{
						bit = 1;
						next_diff = DataBit;
					}
				}
			}
         OneWire_WriteBit(bit);
         *id>>= 1;
         if(bit)
		 	*id |= 0x80;
         DataBit--;
		}
		while(--DataByte);
		id++;
    }
	while(DataBit);
	return next_diff;
}


void OneWire_FindROM(uint8_t *diff, uint8_t id[])
{
	while(1)
    {
		*diff = OneWire_SearchROM(*diff, &id[0]);
		if(*diff==0xFF || *diff==0xFE || *diff==0x00) return;
		return;
    }
}

void DS18B20_GetROM(uint8_t SensorNum, uint8_t *buffer)
{
	int i = 0;
	if(SensorNum>SensorCount || SensorNum==0)return;

	for(i; i<8; i++)
		buffer[i] = DS18B20_SERIAL_NUMBER[SensorNum-1][i];
}

uint8_t DS18B20_ScanBus(void)
{
	uint8_t i;
    uint8_t id[8];
    uint8_t diff;

	SensorCount = 0;

	for(diff=0xFF; diff!=0 && (SensorCount<MAX_SENSORS);)
    {
		OneWire_FindROM(&diff, &id[0]);
		if(diff==0xFF || diff==0xFE) break;

		for (i=0;i<8;i++)
			DS18B20_SERIAL_NUMBER[SensorCount][i] = id[i];

		SensorCount++;
	}
	return SensorCount;
}

*/
