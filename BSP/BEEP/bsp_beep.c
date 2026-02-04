#include "bsp_beep.h"

/* 蜂鸣器控制结构体 */
static struct {
  BeepState_t state;          /* 当前状态 */
  uint8_t     beepCount;      /* 当前响声计数 */
  uint32_t    lastTick;       /* 上次状态切换时间 */
  uint8_t     isEnabled;      /* 蜂鸣器使能标志 */
} beepCtrl = {BEEP_STATE_IDLE, 0, 0, 0};

/**
 * @brief  蜂鸣器初始化
 */
void Beep_Init(void)
{
  beepCtrl.state = BEEP_STATE_IDLE;
  beepCtrl.beepCount = 0;
  beepCtrl.lastTick = 0;
  beepCtrl.isEnabled = 0;
  BEEP_OFF();
}

/**
 * @brief  启动蜂鸣器（间断发声模式）
 */
void Beep_Start(void)
{
  beepCtrl.isEnabled = 1;
  beepCtrl.state = BEEP_STATE_BEEPING;
  beepCtrl.beepCount = 1;
  beepCtrl.lastTick = HAL_GetTick();
  BEEP_ON();
}

/**
 * @brief  停止蜂鸣器
 */
void Beep_Stop(void)
{
  beepCtrl.isEnabled = 0;
  beepCtrl.state = BEEP_STATE_IDLE;
  beepCtrl.beepCount = 0;
  BEEP_OFF();
}

/**
 * @brief  切换蜂鸣器状态
 */
void Beep_Toggle(void)
{
  if (beepCtrl.isEnabled) {
    Beep_Stop();
    } else {
      Beep_Start();
    }
}

/**
 * @brief  查询蜂鸣器是否正在运行
 * @return 1: 运行中, 0: 已停止
 */
uint8_t Beep_IsRunning(void)
{
  return beepCtrl.isEnabled;
}

/**
 * @brief  蜂鸣器处理函数
 * @note   需要在主循环或定时器中断中周期调用（建议1ms调用一次）
 *         实现逻辑：响 -> 间隔 -> 响 -> 间隔 -> 响 -> 暂停 -> 循环
 */
void Beep_Process(void)
{
  uint32_t currentTick;
  uint32_t elapsedTime;

  /* 如果蜂鸣器未启用，直接返回 */
  if (!beepCtrl.isEnabled) {
    return;
  }

  currentTick = HAL_GetTick();
  elapsedTime = currentTick - beepCtrl.lastTick;

  switch (beepCtrl.state) {
    case BEEP_STATE_BEEPING:
      if (elapsedTime >= BEEP_ON_TIME) {
        BEEP_OFF();
        beepCtrl.lastTick = currentTick;

        /* 检查是否已响够三声 */
        if (beepCtrl.beepCount >= BEEP_COUNT_MAX) {
          /* 响完三声后自动停止 */
          Beep_Stop();
        } else {
          beepCtrl.state = BEEP_STATE_INTERVAL;
        }
      }
      break;

    case BEEP_STATE_INTERVAL:
      /* 间隔时间到，开始下一声 */
      if (elapsedTime >= BEEP_OFF_TIME) {
        BEEP_ON();
        beepCtrl.beepCount++;
        beepCtrl.lastTick = currentTick;
        beepCtrl.state = BEEP_STATE_BEEPING;
      }
      break;

    case BEEP_STATE_IDLE:
    default:
      /* 空闲状态，不做处理 */
      break;
  }
}
