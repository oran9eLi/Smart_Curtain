#include "menu.h"


SoftTime_t time;
SensorData_t sensor_data;

// 闪烁控制变量
uint8_t g_blink_state = 0;      // 闪烁状态: 0=显示, 1=隐藏
uint8_t g_setting_hour = 0;     // 当前设置的小时值
static uint32_t blink_timer = 0; // 闪烁定时器

/**
 * @brief  自动模式下的光敏开关页面显示
 * @retval 无
 */
void OLED_autoPageLux(void)
{
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
 * @brief  手动模式下的页面显示
 * @retval 无
 */
void OLED_manualPage(void)
{
  //显示温度
  OLED_ShowChinese(0, 16, 0);
  OLED_ShowChinese(16, 16, 2);
  OLED_ShowChar(32, 16, ':', 16);
  //显示湿度
  OLED_ShowChinese(64, 16, 1);
  OLED_ShowChinese(80, 16, 2);
  OLED_ShowChar(96, 16, ':', 16);

  //显示窗帘状态
  OLED_ShowChinese(0, 32, 30);
  OLED_ShowChinese(16, 32, 31);
  OLED_ShowChinese(32, 32, 61);
  OLED_ShowChinese(48, 32, 62);
  OLED_ShowChar(64, 32, ':', 16);
  //显示窗帘开关状态
  if(Sys_Context.curtainState == OPENED || Sys_Context.curtainState == OPENING)
  {
    OLED_ShowChinese(96, 32, 40);
    OLED_ShowChinese(112, 32, 41);
  }
  else
  {
    OLED_ShowChinese(96, 32, 42);
    OLED_ShowChinese(112, 32, 43);
  }
  OLED_Refresh();
}

/**
 * @brief  显示定时设置页面
 * @retval 无
 */
void OLED_TimeSettingDisplay(void)
{
  if (Sys_Context.focus == FOCUS_OPEN_TIME)
  {
    if (!g_blink_state)
    {
      OLED_ShowNum(0, 48, g_setting_hour, 2, 16);
    }
    else
    {
      OLED_ShowString(0, 48, "  ", 16);
    }
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    OLED_ShowNum(48, 48, Sys_Context.closeHour, 2, 16);
    OLED_ShowString(64, 48, ":00", 16);
  }
  else if (Sys_Context.focus == FOCUS_CLOSE_TIME)
  {
    OLED_ShowNum(0, 48, Sys_Context.openHour, 2, 16);
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    if (!g_blink_state)
    {
      OLED_ShowNum(48, 48, g_setting_hour, 2, 16);
    }
    else
    {
      OLED_ShowString(48, 48, "  ", 16);
    }
    OLED_ShowString(64, 48, ":00", 16);
  }
  else
  {
    OLED_ShowNum(0, 48, Sys_Context.openHour, 2, 16);
    OLED_ShowString(16, 48, ":00", 16);
    OLED_ShowChar(40, 48, '-', 16);
    OLED_ShowNum(48, 48, Sys_Context.closeHour, 2, 16);
    OLED_ShowString(64, 48, ":00", 16);
    if(Sys_Context.openHour > Sys_Context.closeHour)
    {
      OLED_ShowString(100, 48, "+1", 16);
    }
    else
    {
      OLED_ShowString(100, 48, "  ", 16);
    }
  }

  OLED_Refresh();
}

/**
 * @brief  显示系统时间设置页面
 * @retval 无
 */
void OLED_SystemTimeSettingDisplay(void)
{
  extern TempTime_t g_temp_time;
  
  SoftTime_Get(&time);
  
  // 时间显示在第一行正中间 (居中显示)
  OLED_ShowNum(32, 0, g_temp_time.hour, 2, 16);
  OLED_ShowString(48, 0, ":", 16);
  OLED_ShowNum(56, 0, g_temp_time.min, 2, 16);
  OLED_ShowString(72, 0, ":", 16);
  OLED_ShowNum(80, 0, time.sec, 2, 16);
  
  OLED_ShowNum(40, 16, sensor_data.temp, 2, 16);
  OLED_ShowNum(104, 16, sensor_data.humi, 2, 16);
  
  if (Sys_Context.focus == FOCUS_HOUR)
  {
    if (g_blink_state)
    {
      OLED_ShowString(32, 0, "  ", 16);
    }
  }
  else if (Sys_Context.focus == FOCUS_MIN)
  {
    if (g_blink_state)
    {
      OLED_ShowString(56, 0, "  ", 16);
    }
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

  OLED_ShowNum(32, 0, time.hour, 2, 16);
  OLED_ShowString(48, 0, ":", 16);
  OLED_ShowNum(56, 0, time.min, 2, 16);
  OLED_ShowString(72, 0, ":", 16);
  OLED_ShowNum(80, 0, time.sec, 2, 16);

  OLED_ShowNum(40, 16, sensor_data.temp, 2, 16);
  OLED_ShowNum(104, 16, sensor_data.humi, 2, 16);
  OLED_ShowNum(72, 32, sensor_data.lux, 2, 16);
  OLED_ShowChar(96, 32, '%', 16);
  if(sensor_data.CO == GPIO_PIN_SET)
  {
    OLED_ShowChinese(72, 48, 51);
    OLED_ShowChinese(88, 48, 52);
  }
  else if(sensor_data.CO == GPIO_PIN_RESET)
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

  OLED_ShowNum(32, 0, time.hour, 2, 16);
  OLED_ShowString(48, 0, ":", 16);
  OLED_ShowNum(56, 0, time.min, 2, 16);
  OLED_ShowString(72, 0, ":", 16);
  OLED_ShowNum(80, 0, time.sec, 2, 16);

  OLED_ShowNum(40, 16, sensor_data.temp, 2, 16);
  OLED_ShowNum(104, 16, sensor_data.humi, 2, 16);

  OLED_Refresh();
}


/**
 * @brief  更新用户界面
 * @retval 无
 */
void UI_Update(void)
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
    if (Global_State == FSM_SET_SYSTEM_TIME)
    {
      OLED_SystemTimeSettingDisplay();
    }
    else
    {
      switch (Sys_Context.mode)
      {
        case MODE_AUTO_LUX:
          OLED_SensorDataDisplay1();
          break;
        case MODE_AUTO_TIM:
          OLED_SensorDataDisplay2();
          OLED_TimeSettingDisplay();
          break;
        case MODE_MANUAL:
          OLED_SensorDataDisplay2();
          break;
        default:
          break;
      }
    }
  }
}
