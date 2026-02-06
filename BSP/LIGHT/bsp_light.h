#ifndef __BSP_LIGHT_H
#define __BSP_LIGHT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "adc.h"

#define LIGHT_ADC_CHANNEL   0
#define ADC_CHANNEL_COUNT   1
#define ADC_VALUE_MAX       4095

#ifndef LIGHT_INVERT
#define LIGHT_INVERT 0
#endif

typedef enum {
  LIGHT_STATE_DARK = 0,
  LIGHT_STATE_DIM,
  LIGHT_STATE_BRIGHT
} Light_State_t;

void Light_Init(void);
uint8_t Light_Read_Percent(void);
Light_State_t Light_Get_State(void);

#ifdef __cplusplus
}
#endif

#endif
