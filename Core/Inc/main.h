/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

typedef enum { MODE_AUTO_LUX, MODE_AUTO_TIM, MODE_MANUAL} SysMode_t;//系统模式枚举类型
typedef enum { CLOSED, OPENED, CLOSING, OPENING } CurtainState_t;//窗帘状态枚举类型
typedef enum { LUX_LOW, LUX_HIGH } LuxState_t;//光照状态枚举类型
typedef enum { CO_LOW, CO_HIGH } COState_t;//CO状态枚举类型
typedef enum { FOCUS_NONE, FOCUS_OPEN_TIME, FOCUS_CLOSE_TIME, FOCUS_HOUR, FOCUS_MIN } SettingFocus_t;//聚焦枚举类型

typedef struct
{
  SysMode_t mode;//系统模式
  CurtainState_t curtainState;//窗帘状态
  LuxState_t luxState;//光照状态
  COState_t coState;//CO状态
  uint8_t openHour;//打开时间(小时)
  uint8_t closeHour;//关闭时间(小时)
  SettingFocus_t focus;//聚焦状态
}SysStatus_t;//系统状态枚举类型

// 系统时间设置临时变量（用于手动模式设置系统时间）
typedef struct
{
  uint8_t hour;
  uint8_t min;
} TempTime_t;

extern TempTime_t g_temp_time;

typedef enum
{
  FSM_IDLE_LUX,
  FSM_IDLE_TIM,
  FSM_IDLE_MANUAL,
  FSM_OPENING,
  FSM_CLOSING,
  FSM_SET_OPEN_TIME,
  FSM_SET_CLOSE_TIME,
  FSM_SET_SYSTEM_TIME,//设置系统时间状态
  FSM_ERROR,
}FSMState_t;//有限状态机状态枚举类型

typedef enum
{
  CMD_NONE = 0,//无命令
  CMD_MODE,//模式切换（按键1）
  CMD_INC,//时间增加（定时模式，按键2）
  CMD_OPEN,//手动打开（手动模式，按键2）
  CMD_DEC,//时间减少（定时模式，按键3）
  CMD_CLOSE,//手动关闭（手动模式，按键3）
  CMD_PAUSE,//暂停（自动模式，按键4）
  CMD_ENTER,//确认（定时模式，按键4）
  CMD_SET_TIME,//设置系统时间（手动模式，按键4）
  //CMD_INCH,//寸动（手动模式，按键4）
}UserCMD_t;//用户命令枚举类型

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BUZZER_Pin GPIO_PIN_13
#define BUZZER_GPIO_Port GPIOC
#define Light_Pin GPIO_PIN_0
#define Light_GPIO_Port GPIOA
#define MQ7_Pin GPIO_PIN_1
#define MQ7_GPIO_Port GPIOA
#define BT_Pin GPIO_PIN_2
#define BT_GPIO_Port GPIOA
#define BTA3_Pin GPIO_PIN_3
#define BTA3_GPIO_Port GPIOA
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOB
#define JR6001_Pin GPIO_PIN_10
#define JR6001_GPIO_Port GPIOB
#define JR6001B11_Pin GPIO_PIN_11
#define JR6001B11_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOB
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOB
#define KEY3_Pin GPIO_PIN_14
#define KEY3_GPIO_Port GPIOB
#define KEY4_Pin GPIO_PIN_15
#define KEY4_GPIO_Port GPIOB
#define Debug_Pin GPIO_PIN_9
#define Debug_GPIO_Port GPIOA
#define DebugA10_Pin GPIO_PIN_10
#define DebugA10_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_11
#define DHT11_GPIO_Port GPIOA
#define MOTOR_IN3_Pin GPIO_PIN_12
#define MOTOR_IN3_GPIO_Port GPIOA
#define MOTOR_IN4_Pin GPIO_PIN_15
#define MOTOR_IN4_GPIO_Port GPIOA
#define OLED_Pin GPIO_PIN_6
#define OLED_GPIO_Port GPIOB
#define OLEDB7_Pin GPIO_PIN_7
#define OLEDB7_GPIO_Port GPIOB
#define MOTOR_IN1_Pin GPIO_PIN_8
#define MOTOR_IN1_GPIO_Port GPIOB
#define MOTOR_IN2_Pin GPIO_PIN_9
#define MOTOR_IN2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
