#ifndef __SOFT_TIME_H
#define __SOFT_TIME_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "event.h"

typedef struct
{
  uint8_t hour;
  uint8_t min;
  uint8_t sec;
} SoftTime_t;

extern SysStatus_t Sys_Context;

void SoftTime_Init(uint8_t hour, uint8_t min, uint8_t sec);
void SoftTime_Tick_1ms(void);
void SoftTime_Get(SoftTime_t *t);
void SoftTime_Set(uint8_t hour, uint8_t min, uint8_t sec);
void SoftTime_CTRL(void);
#endif
