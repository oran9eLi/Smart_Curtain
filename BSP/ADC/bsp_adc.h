#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "main.h"
#include "adc.h"

#define ADC_CHANNEL_COUNT  2
#define ADC_VALUE_MAX      4095

void ADC_Init(void);
uint16_t ADC_GetValue(uint8_t channel);

#endif