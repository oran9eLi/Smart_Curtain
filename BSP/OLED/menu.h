#ifndef __MENU_H__
#define __MENU_H__

#include "main.h"
#include "bsp_oled.h"
#include "sensor.h"
#include "soft_time.h"
#include "stdio.h"

extern SysStatus_t Sys_Context;
extern FSMState_t Global_State;
#define UI_REFRESH_RATE  300//刷新时间间隔（毫秒）

// 闪烁控制
#define BLINK_INTERVAL  500  // 闪烁间隔（毫秒）
extern uint8_t g_blink_state;  // 0=显示, 1=隐藏
extern uint8_t g_setting_hour;  // 当前设置的值
extern uint8_t g_setting_type;  // 0=打开时间, 1=关闭时间

void OLED_autoPageLux(void);
void OLED_autoPageTim(void);
void OLED_manualPage(void);
void OLED_SensorDataDisplay1(void);
void OLED_SensorDataDisplay2(void);
void OLED_TimeSettingDisplay(uint8_t is_setting_open);  // 设置时间显示(带闪烁)
void UI_Update_WithBlink(void);  // 更新用户界面(包含闪烁控制)

#endif
