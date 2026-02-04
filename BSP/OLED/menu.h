#ifndef __MENU_H__
#define __MENU_H__

#include "main.h"
#include "bsp_oled.h"
#include "sensor.h"
#include "soft_time.h"
#include "stdio.h"

extern SysStatus_t Sys_Context;
#define UI_REFRESH_RATE  300//刷新时间间隔（毫秒）

void OLED_autoPageLux(void);
void OLED_autoPageTim(void);
void OLED_manualPage(void);
void OLED_SensorDataDisplay1(void);
void OLED_SensorDataDisplay2(void);
void UI_Update(void);

#endif
