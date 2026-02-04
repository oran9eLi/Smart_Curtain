#include "bsp_led.h"

void LED_Init(void)
{
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void LED_On(void)
{
  LED_ON();
}

void LED_Off(void)
{
  LED_OFF();
}

void LED_Toggle(void)
{
  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}
