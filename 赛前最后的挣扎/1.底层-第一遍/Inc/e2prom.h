#ifndef E2PROM_H
#define E2PROM_H

#include "stm32f10x.h"
#include "i2c.h"


void E2Write(u8 addr, u8 dat);
u8 E2Read(u8 addr);

#endif
