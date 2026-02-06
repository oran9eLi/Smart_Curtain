#include "bsp_mq7.h"

void MQ7_Init(void)
{

}

uint8_t MQ7_Read_Status(void)
{
  return HAL_GPIO_ReadPin(MQ7_GPIO_Port, MQ7_Pin);
}
