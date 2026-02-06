#include "bsp_light.h"

static uint16_t adc_buffer[ADC_CHANNEL_COUNT] = {0};

void Light_Init(void)
{
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, ADC_CHANNEL_COUNT);
}

static uint16_t Light_MapPercent(uint16_t raw)
{
    uint32_t v = raw;
    if (v > ADC_VALUE_MAX) v = ADC_VALUE_MAX;
#if (LIGHT_INVERT)
    uint32_t pct = (v * 100u) / ADC_VALUE_MAX;
#else
    uint32_t pct = ((ADC_VALUE_MAX - v) * 100u) / ADC_VALUE_MAX;
#endif
    return (uint16_t)pct;
}

uint8_t Light_Read_Percent(void)
{
    uint16_t raw_value = adc_buffer[LIGHT_ADC_CHANNEL];
    return (uint8_t)Light_MapPercent(raw_value);
}

Light_State_t Light_Get_State(void)
{
    static Light_State_t last = LIGHT_STATE_DIM;
    uint8_t pct = Light_Read_Percent();

    switch (last)
    {
        case LIGHT_STATE_DARK:
            if (pct > 30) last = LIGHT_STATE_DIM;
            break;
        case LIGHT_STATE_DIM:
            if (pct < 20) last = LIGHT_STATE_DARK;
            else if (pct > 70) last = LIGHT_STATE_BRIGHT;
            break;
        case LIGHT_STATE_BRIGHT:
            if (pct < 60) last = LIGHT_STATE_DIM;
            break;
        default:
            last = LIGHT_STATE_DIM;
            break;
    }
    return last;
}
