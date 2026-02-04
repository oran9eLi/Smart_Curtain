#ifndef __BSP_KEY_H
#define __BSP_KEY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "event.h"

// 按键ID枚举
typedef enum {
  KEY_1 = 0,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_COUNT = 4,
  KEY_NONE = 0xFF
} Key_ID_t;

// 按键状态枚举
typedef enum {
  KEY_STATE_IDLE,
  KEY_STATE_DEBOUNCE,
  KEY_STATE_PRESSED,
} Key_State_t;

// 按键结构体
typedef struct {
  Key_State_t state;
  uint8_t counter;
} Key_t;

#define KEY_SCAN_INTERVAL   10//按键扫描间隔10ms
#define KEY_DEBOUNCE_COUNTER   2//按键消抖计数器，消抖时间 = 2 * KEY_SCAN_INTERVAL

void Key_Init(void);
void Key_Scan(void);
uint8_t Key_GetLevel(Key_ID_t key_id);

#ifdef __cplusplus
}
#endif

#endif