//#ifndef __COMTERM
//#define __COMTERM

#ifndef COMTERM_H_INCLUDED
#define COMTERM_H_INCLUDED


#include "ch.h"
#include "hal.h"
#include <stdio.h>
#include <string.h>

int wconfInit(void);

int wifiInit(void);

void wifiWriteDATA(uint8_t *WDATA[] );

uint8_t pubA(void);
uint8_t pubH(void);
#endif
