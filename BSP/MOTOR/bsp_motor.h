#ifndef __BSP_MOTOR_H
#define __BSP_MOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "event.h"

/* 电机方向枚举 */
typedef enum {
  MOTOR_STOP_MODE = 0,
  MOTOR_CW,  // 顺时针
  MOTOR_CCW  // 逆时针
} Motor_Dir_t;

#define MOTOR_A 				GPIO_PIN_8
#define MOTOR_B 				GPIO_PIN_9
#define MOTOR_C 				GPIO_PIN_12
#define MOTOR_D 				GPIO_PIN_15
#define MOTOR_AB_PORT 			GPIOB
#define MOTOR_CD_PORT 			GPIOA

/* 函数声明 */
void Motor_Start(Motor_Dir_t dir);
void Motor_Rotate_Angle(Motor_Dir_t dir, uint16_t angle);
void Motor_Stop(void);
void MotorStep_Process(void); // 放在定时器中断中调用

#ifdef __cplusplus
}
#endif

#endif /* __BSP_MOTOR_H */
