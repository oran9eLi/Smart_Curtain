#include "soft_time.h"

static SoftTime_t sys_time;
static uint16_t ms_cnt = 0;

/**
 * @brief 初始化软件定时器
 * 
 * @param hour 时
 * @param min 分
 * @param sec 秒
 */
void SoftTime_Init(uint8_t hour, uint8_t min, uint8_t sec)
{
  sys_time.hour = hour % 24;
  sys_time.min  = min  % 60;
  sys_time.sec  = sec  % 60;
  ms_cnt = 0;
}

/**
 * @brief 软件定时器 tick 1ms
 * 
 */
void SoftTime_Tick_1ms(void)
{
  ms_cnt++;
  if (ms_cnt >= 1000)
  {
    ms_cnt = 0;
    sys_time.sec++;

    if (sys_time.sec >= 60)
    {
      sys_time.sec = 0;
      sys_time.min++;
    }

    if (sys_time.min >= 60)
    {
      sys_time.min = 0;
      sys_time.hour++;
    }

    if (sys_time.hour >= 24)
    {
      sys_time.hour = 0;
    }
  }
}

/**
 * @brief 获取软件定时器时间
 * 
 * @param t 时间结构体指针
 */
void SoftTime_Get(SoftTime_t *t)
{
  if (!t) return;

  __disable_irq();
  *t = sys_time;
  __enable_irq();
}

/**
 * @brief 设置软件定时器时间
 * 
 * @param hour 时
 * @param min 分
 * @param sec 秒
 */
void SoftTime_Set(uint8_t hour, uint8_t min, uint8_t sec)
{
  __disable_irq();// 禁用中断，确保时间设置的原子性
  sys_time.hour = hour % 24;
  sys_time.min  = min  % 60;
  sys_time.sec  = sec  % 60;
  __enable_irq();// 使能中断
}

/**
 * @brief 软件定时器控制函数
 * @retval 无
 */
void SoftTime_CTRL(void)
{
  static uint8_t last_triggered_hour = 255;  // 标志位：记录上次触发的小时数
  
  if (Sys_Context.mode == MODE_AUTO_TIM)
  {
    // 只在整点（分=0,秒=0）且该小时未触发过时才执行
    if (sys_time.min == 0 && sys_time.sec == 0 && sys_time.hour != last_triggered_hour)
    {
      last_triggered_hour = sys_time.hour;  // 更新标志位，标记该小时已触发
      
      if (sys_time.hour == Sys_Context.closeHour)
      {
        Event_Queue(EVT_TIMER_CTRL, 0);  // 关闭时间：入队关闭事件
      }
      else if (sys_time.hour == Sys_Context.openHour)
      {
        Event_Queue(EVT_TIMER_CTRL, 1);  // 打开时间：入队打开事件
      }
    }
  }
}