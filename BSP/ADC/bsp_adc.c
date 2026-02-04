#include "bsp_adc.h"

static uint16_t adc_buffer[ADC_CHANNEL_COUNT] = {0};

void ADC_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);//校准
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_CHANNEL_COUNT);
}

uint16_t ADC_GetValue(uint8_t channel)
{
    if (channel >= ADC_CHANNEL_COUNT)
    {
        return 0;
    }
    return adc_buffer[channel];
}