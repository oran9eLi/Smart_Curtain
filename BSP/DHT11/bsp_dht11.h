#ifndef __BSP_DHT11_H
#define __BSP_DHT11_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* DHT11 引脚定义 (使用 main.h 中的宏) */
/* 
   注意：需要在 CubeMX 中将 PA11 配置为 GPIO_Input 或 GPIO_Output 均可，
   代码中会动态切换方向。
*/
#ifndef DHT11_Pin
#define DHT11_Pin       GPIO_PIN_11
#endif

#ifndef DHT11_GPIO_Port
#define DHT11_GPIO_Port GPIOA
#endif

/* 函数声明 */
void DHT11_Init(void);
uint8_t DHT11_Read_Data(float *temp, float *humi);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DHT11_H */
