#include "bsp_mq7.h"

void MQ7_Init(void)
{
}

uint8_t MQ7_Read_Percent(void)
{
    uint16_t adc_value = ADC_GetValue(MQ7_ADC_CHANNEL);
    
    if (adc_value > ADC_VALUE_MAX)
    {
        adc_value = ADC_VALUE_MAX;
    }
    
    uint8_t percent = (ADC_VALUE_MAX - adc_value) * 100 / ADC_VALUE_MAX;
    
    return percent;
}