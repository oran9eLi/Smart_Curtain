#include "menu.h"


SoftTime_t time;
SensorData_t sensor_data;

// 闪烁控制变量
uint8_t g_blink_state = 0;      // 闪烁状态: 0=显示, 1=隐藏
uint8_t g_setting_hour = 0;     // 当前设置的小时值
uint8_t g_setting_type = 0;     // 设置类型: 0=打开时间, 1=关闭时间
static uint32_t blink_timer = 0; // 闪烁定时器

/**
 * @brief  自动模式下的光敏开关页面显示
 * @retval 无
 */
void OLED_autoPageLux(void)
{
  //显示时间
  OLED_ShowString(0, 0, "Time:", 16);
  //显示温度
  OLED_ShowChinese(0, 16, 0);
  OLED_ShowChinese(16, 16, 2);
  OLED_ShowChar(32, 16, ':', 16);
  //显示湿度
  OLED_ShowChinese(64, 16, 1);
  OLED_ShowChinese(80, 16, 2);
  OLED_ShowChar(96, 16, ':', 16);
  //显示光照
  OLED_ShowChinese(0, 32, 3);
  OLED_ShowChinese(16, 32, 4);
  OLED_ShowChinese(32, 32, 5);
  OLED_ShowChinese(48, 32, 2);
  OLED_ShowChar(64, 32, ':', 16);
  //显示CO
  OLED_ShowChinese(0, 48, 18);
  OLED_ShowChinese(16, 48, 19);
  OLED_ShowChinese(32, 48, 20);
  OLED_ShowChinese(48, 48, 21);
  OLED_ShowChar(64, 48, ':', 16);

  OLED_Refresh();
}

/**
 * @brief  自动模式下的定时页面显示
 * @retval 无
 */
void OLED_autoPageTim(void)
{
  //显示时间
  OLED_ShowString(0, 0, "Time:", 16);
  //显示温度
  OLED_ShowChinese(0, 16, 0);
  OLED_ShowChinese(16, 16, 2);
  OLED_ShowChar(32, 16, ':', 16);
  //显示湿度
  OLED_ShowChinese(64, 16, 1);
  OLED_ShowChinese(80, 16, 2);
  OLED_ShowChar(96, 16, ':', 16);
  //显示"定时时间"
  OLED_ShowChinese(0, 32, 57);
  OLED_ShowChinese(16, 32, 58);
  OLED_ShowChinese(32, 32, 58);
  OLED_ShowChinese(48, 32, 54);

  OLED_Refresh();
}

/**
 * @brief  显示带闪烁的时间设置(设置模式专用)
 * @param  is_setting_open: 0=显示打开时间设置, 1=显示关闭时间设置
 * @retval 无
 */
void OLED_TimeSettingDisplay(uint8_t is_setting_open)
{
  // 显示格式: XX:00 - XX:00
  // is_setting_open: 0=设置打开时间, 1=设置关闭时间, 2=正常显示(不闪烁)
  
  if (is_setting_open == 0) {
    // 正在设置打开时间
    // 打开时间闪烁显示
    if (!g_blink_state) {
      OLED_ShowNum(0, 48, g_setting_hour, 2, 16);
    } else {
      // 闪烁时显示空格或清除
      OLED_ShowString(0, 48, "  ", 16);
    }
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    // 关闭时间正常显示
    OLED_ShowNum(48, 48, Sys_Context.closeHour, 2, 16);
    OLED_ShowString(64, 48, ":00", 16);
  } else if (is_setting_open == 1) {
    // 正在设置关闭时间
    // 打开时间正常显示(使用已设置的值或当前值)
    OLED_ShowNum(0, 48, Sys_Context.openHour, 2, 16);
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    // 关闭时间闪烁显示
    if (!g_blink_state) {
      OLED_ShowNum(48, 48, g_setting_hour, 2, 16);
    } else {
      OLED_ShowString(48, 48, "  ", 16);
    }
    OLED_ShowString(64, 48, ":00", 16);
  } else {
    // 正常显示模式(不闪烁)
    OLED_ShowNum(0, 48, Sys_Context.openHour, 2, 16);
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    OLED_ShowNum(48, 48, Sys_Context.closeHour, 2, 16);
    OLED_ShowString(64, 48, ":00", 16);
  }
  
  OLED_Refresh();
}

/**
 * @brief  显示传感器数据
 * @retval 无
 */
void OLED_SensorDataDisplay1(void)
{
  SoftTime_Get(&time);
  char all_data[128] = {0};
  sprintf(all_data, "时间:%02d:%02d:%02d 温度:%.1fC 湿度:%.1f%% 光照:%d",
          time.hour, time.min, time.sec,  
          sensor_data.temp, sensor_data.humi, sensor_data.lux);

  OLED_ShowNum(40, 0, time.hour, 2, 16);
  OLED_ShowString(56, 0, ":", 16);
  OLED_ShowNum(64, 0, time.min, 2, 16);
  OLED_ShowString(80, 0, ":", 16);
  OLED_ShowNum(88, 0, time.sec, 2, 16);

  OLED_ShowNum(40, 16, sensor_data.temp, 2, 16);
  OLED_ShowNum(104, 16, sensor_data.humi, 2, 16);
  OLED_ShowNum(72, 32, sensor_data.lux, 2, 16);
  OLED_ShowChar(96, 32, '%', 16);
  if(sensor_data.CO < 500)
  {
    OLED_ShowChinese(72, 48, 51);
    OLED_ShowChinese(88, 48, 52);
  }
  else
  {
    OLED_ShowChinese(72, 48, 50);
    OLED_ShowChinese(88, 48, 52);
  }
  OLED_Refresh();
}

void OLED_SensorDataDisplay2(void)
{
  SoftTime_Get(&time);
  char all_data[128] = {0};
  sprintf(all_data, "时间:%02d:%02d:%02d 温度:%.1fC 湿度:%.1f%%",
          time.hour, time.min, time.sec,  
          sensor_data.temp, sensor_data.humi);

  OLED_ShowNum(40, 0, time.hour, 2, 16);
  OLED_ShowString(56, 0, ":", 16);
  OLED_ShowNum(64, 0, time.min, 2, 16);
  OLED_ShowString(80, 0, ":", 16);
  OLED_ShowNum(88, 0, time.sec, 2, 16);

  OLED_ShowNum(40, 16, sensor_data.temp, 2, 16);
  OLED_ShowNum(104, 16, sensor_data.humi, 2, 16);
  OLED_Refresh();
}

/**
 * @brief  手动模式下的页面显示
 * @retval 无
 */
void OLED_manualPage(void)
{
  //显示时间
  OLED_ShowString(0, 0, "Time:", 16);
  //显示温度
  OLED_ShowChinese(0, 16, 0);
  OLED_ShowChinese(16, 16, 2);
  OLED_ShowChar(32, 16, ':', 16);
  //显示湿度
  OLED_ShowChinese(64, 16, 1);
  OLED_ShowChinese(80, 16, 2);
  OLED_ShowChar(96, 16, ':', 16);

  OLED_Refresh();
}

/**
 * @brief  更新用户界面
 * @retval 无
 */
void UI_Update_WithBlink(void)
{
  static uint32_t timer = 0;
  static uint8_t last_mode = 0xFF;
  static uint8_t last_fsm_state = 0xFF;

  // 处理闪烁定时器
  if (HAL_GetTick() - blink_timer >= BLINK_INTERVAL)
  {
    blink_timer = HAL_GetTick();
    g_blink_state = !g_blink_state;  // 切换闪烁状态
  }

  // 检查模式或FSM状态变化
  if (Sys_Context.mode != last_mode || Global_State != last_fsm_state)
  {
    OLED_Clear();

    // 根据当前模式绘制基础页面
    switch (Sys_Context.mode)
    {
      case MODE_AUTO_LUX:
        OLED_autoPageLux();
        break;
      case MODE_AUTO_TIM:
        OLED_autoPageTim();  // 只显示基础布局，不显示时间值
        break;
      case MODE_MANUAL:
        OLED_manualPage();
        break;
      default:
        break;
    }
    last_mode = Sys_Context.mode;
    last_fsm_state = Global_State;
    timer = 0;
  }

  // 定期刷新显示(300ms)
  if (HAL_GetTick() - timer >= UI_REFRESH_RATE)
  {
    timer = HAL_GetTick();
    
    // 首先刷新传感器数据(温度、湿度等)
    switch (Sys_Context.mode)
    {
      case MODE_AUTO_LUX:
        OLED_SensorDataDisplay1();
        break;
      case MODE_AUTO_TIM:
        OLED_SensorDataDisplay2();
        // 在定时模式下，根据FSM状态显示时间设置
        if (Global_State == FSM_SET_OPEN_TIME) {
          OLED_TimeSettingDisplay(0);  // 显示打开时间设置(闪烁)
        } else if (Global_State == FSM_SET_CLOSE_TIME) {
          OLED_TimeSettingDisplay(1);  // 显示关闭时间设置(闪烁)
        } else {
          // 正常显示当前设置的时间(不闪烁)
          OLED_TimeSettingDisplay(2);  // 2表示正常显示模式
        }
        break;
      case MODE_MANUAL:
        OLED_SensorDataDisplay2();
        break;
      default:
        break;
    }
  }
}