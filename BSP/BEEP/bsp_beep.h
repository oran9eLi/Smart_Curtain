#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* 蜂鸣器硬件控制宏 */
#define BEEP_ON()  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)

/* 蜂鸣器时间参数配置（单位：毫秒） */
#define BEEP_ON_TIME      200   /* 单次响声持续时间 */
#define BEEP_OFF_TIME     200   /* 响声间隔时间 */
#define BEEP_PAUSE_TIME   800   /* 三声后的暂停时间 */
#define BEEP_COUNT_MAX    3     /* 连续响声次数 */

/* 蜂鸣器状态枚举 */
typedef enum {
  BEEP_STATE_IDLE = 0,        /* 空闲状态（关闭） */
  BEEP_STATE_BEEPING,         /* 正在响 */
  BEEP_STATE_INTERVAL,        /* 响声间隔 */
  BEEP_STATE_PAUSE            /* 三声后暂停 */
} BeepState_t;

/* 函数声明 */
void Beep_Init(void);
void Beep_Start(void);
void Beep_Stop(void);
void Beep_Toggle(void);
uint8_t Beep_IsRunning(void);
void Beep_Process(void);        /* 需在主循环或定时器中周期调用（建议1ms） */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_BEEP_H */
