#ifndef __BSP_MQ7_H
#define __BSP_MQ7_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "bsp_adc.h"

#define MQ7_ADC_CHANNEL  1

void MQ7_Init(void);
uint8_t MQ7_Read_Percent(void);

#ifdef __cplusplus
}
#endif

#endif