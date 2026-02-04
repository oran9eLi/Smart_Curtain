#include "menu.h"


SoftTime_t time;
SensorData_t sensor_data;

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
  //显示“定时时间”
  OLED_ShowChinese(0, 32, 57);
  OLED_ShowChinese(16, 32, 58);
  OLED_ShowChinese(32, 32, 58);
  OLED_ShowChinese(48, 32, 54);

  OLED_ShowNum(0, 48, Sys_Context.openTime, 2, 16);
  OLED_ShowString(16, 0, ":", 16);
  OLED_ShowNum(24, 0, "00", 2, 16);
  OLED_ShowChar(40, 0, '-', 16);
  OLED_ShowNum(48, 0, Sys_Context.closeTime, 2, 16);
  OLED_ShowString(64, 0, ":", 16);
  OLED_ShowNum(72, 0, "00", 2, 16);

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
void UI_Update(void)
{
  static uint32_t timer = 0;
  static uint8_t last_mode = 0xFF;

  if(Sys_Context.mode != last_mode)
  {
    OLED_Clear();

    switch (Sys_Context.mode)
    {
      case MODE_AUTO_LUX:
        OLED_autoPageLux();
        break;
      case MODE_AUTO_TIM:
        OLED_autoPageTim();
        break;
      case MODE_MANUAL:
        OLED_manualPage();
        break;
      default:
        break;
    }
    last_mode = Sys_Context.mode;
    timer = 0;
  }

  if(HAL_GetTick() - timer >= UI_REFRESH_RATE)
  {
    timer = HAL_GetTick();
    
    switch(Sys_Context.mode)
    {
      case MODE_AUTO_LUX:
        OLED_SensorDataDisplay1();
        break;
      case MODE_AUTO_TIM:
        OLED_SensorDataDisplay2();
        break;
      case MODE_MANUAL:
        OLED_SensorDataDisplay2();
        break;
      default:
        break;
    }
  }
}