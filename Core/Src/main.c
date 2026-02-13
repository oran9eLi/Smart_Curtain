/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "bsp_dht11.h"
#include "bsp_key.h"
#include "bsp_motor.h"
#include "bsp_oled.h"
#include "menu.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_jr6001.h"
#include "sensor.h"
#include "soft_time.h"
#include "event.h"
#include "bsp_bt.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//系统环境状态
SysStatus_t Sys_Context = {
  .mode = MODE_MANUAL,
  .curtainState = OPENED,
  .luxState = LUX_HIGH,
  .coState = CO_LOW,
  .openHour = 0,
  .closeHour = 0,
  .focus = FOCUS_NONE 
};
FSMState_t Global_State = FSM_IDLE_MANUAL;//有限状态机状态
extern uint8_t g_setting_hour;     // 当前设置的小时值(定义在menu.c)
TempTime_t g_temp_time = {0, 0, 0}; // 系统时间设置临时变量
SoftTime_t current_time = {0, 0, 0}; // 当前系统时间

Event_t evt = {.type = EVT_NONE, .param = 0};

// 计数器与标志位
uint32_t key_scan_tick = 0;
uint32_t led_tick = 0;
uint32_t time_ctrl_tick = 0;
uint32_t sensor_tick = 0;
uint32_t bt_tick = 0;
uint8_t sensorscan_flag = 0;
uint8_t btprocess_flag = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Handle_Idle_Lux(Event_t *evt);//处理空闲状态事件(光照模式)
void Handle_Idle_Tim(Event_t *evt);//处理空闲状态事件(时间模式)
void Handle_Idle_Manual(Event_t *evt);//处理空闲状态事件(手动模式)
void Handle_Opening(Event_t *evt);//处理打开状态事件
void Handle_Closing(Event_t *evt);//处理关闭状态事件
void Handle_SetOpenTime(Event_t *evt);//处理设置打开时间状态事件
void Handle_SetCloseTime(Event_t *evt);//处理设置关闭时间状态事件
void Handle_SetSystemTime(Event_t *evt);//处理设置系统时间状态事件
void Handle_Error(Event_t *evt);//处理错误状态事件

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Mode_Change(void)
{
  if(Sys_Context.mode == MODE_AUTO_LUX)
  {
    Sys_Context.mode = MODE_AUTO_TIM;
    Global_State = FSM_IDLE_TIM;
  }
  else if(Sys_Context.mode == MODE_AUTO_TIM)
  {
    Sys_Context.mode = MODE_MANUAL;
    Global_State = FSM_IDLE_MANUAL;
  }
  else if(Sys_Context.mode == MODE_MANUAL)
  {
    Sys_Context.mode = MODE_AUTO_LUX;
    Global_State = FSM_IDLE_LUX;
  }
}
void Curtain_Open(void)
{
    Global_State = FSM_OPENING;
    Sys_Context.curtainState = OPENING;
    Motor_Rotate_Angle(MOTOR_CW, 360);
    JR6001_Play(2);
}
void Curtain_Close(void)
{
    Global_State = FSM_CLOSING;
    Sys_Context.curtainState = CLOSING;
    Motor_Rotate_Angle(MOTOR_CCW, 360);
    JR6001_Play(3);
}
void Enter_SystemTime_Setting(void)
{
  Global_State = FSM_SET_SYSTEM_TIME;
  Sys_Context.focus = FOCUS_HOUR;
}
/**
 * @brief  按键映射表
 * @param  key 按键代码
 * @param  mode 当前模式
 * @retval 用户命令枚举值
 */
UserCMD_t Map_Key_To_Cmd(uint8_t key, uint8_t mode)
{
  if(mode == MODE_AUTO_LUX)
  {
    switch(key)
    {
      case KEY_1: return CMD_MODE;
      default:    return CMD_NONE;
    }
  }
  else if(mode == MODE_AUTO_TIM)
  {
    switch(key)
    {
      case KEY_1: return CMD_MODE;
      case KEY_2: return CMD_INC;
      case KEY_3: return CMD_DEC;
      case KEY_4: return CMD_ENTER;
      default:    return CMD_NONE;
    }
  }
  else if(mode == MODE_MANUAL)
  {
    switch(key)
    {
      case KEY_1: return CMD_MODE;
      case KEY_2: return CMD_OPEN;
      case KEY_3: return CMD_CLOSE;
      case KEY_4: return CMD_SET_TIME;
      default:    return CMD_NONE;
    }
  }
  else
  {
    return CMD_NONE;
  }
}

/**
 * @brief  系统事件分发
 * @param  evt 事件指针
 * @retval None
 */
void System_Dispatch(Event_t *evt)
{
  //应急处理CO传感器事件
  if(evt->type == EVT_SENSOR_CO && evt->param == 1)
  {
    Sys_Context.coState = CO_HIGH;
    Beep_Start();
    if(Sys_Context.curtainState != OPENED && Global_State != FSM_OPENING)
    {
      Curtain_Open();
      return;
    }
  }
  // 处理蓝牙事件
  if(evt->type == EVT_BT_CMD)
  {
    BT_CmdPacket_t packet;
    packet.cmd = (BT_Command_t)(evt->param & 0xFF);
    packet.param[0]  = (evt->param >> 8)  & 0xFF;
    packet.param[1]  = (evt->param >> 16) & 0xFF;
    packet.param[2]  = (evt->param >> 24) & 0xFF;
    BT_ExecuteCommand(&packet);
    return;
  }

  //常规事件分发
  switch (Global_State)
  {
    case FSM_IDLE_LUX:        Handle_Idle_Lux(evt);     break;
    case FSM_IDLE_TIM:        Handle_Idle_Tim(evt);     break;
    case FSM_IDLE_MANUAL:     Handle_Idle_Manual(evt);  break;
    case FSM_OPENING:         Handle_Opening(evt);      break;
    case FSM_CLOSING:         Handle_Closing(evt);      break;
    case FSM_SET_OPEN_TIME:   Handle_SetOpenTime(evt);  break;
    case FSM_SET_CLOSE_TIME:  Handle_SetCloseTime(evt); break;
    case FSM_SET_SYSTEM_TIME: Handle_SetSystemTime(evt);break;
    case FSM_ERROR:           Handle_Error(evt);        break;
    default: break;
  }
}

void Handle_Idle_Lux(Event_t *evt)
{
  UserCMD_t cmd;
  switch(evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch(cmd)
      {
        case CMD_MODE: Mode_Change(); break;
        default: break;
      }
      break;
    case EVT_SENSOR_LIGHT:
      if(evt->param == 1)
      {
        Sys_Context.luxState = LUX_HIGH;
        if(Sys_Context.curtainState == CLOSED)
        {
          Sys_Context.curtainState = OPENING;
          Curtain_Open();
        }
      }
      else
      {
        Sys_Context.luxState = LUX_LOW;
        if(Sys_Context.curtainState == OPENED)
        {
          Sys_Context.curtainState = CLOSING;
          Curtain_Close();
        }
      }
      break;
  }
}

void Handle_Idle_Tim(Event_t *evt)
{
  UserCMD_t cmd;
  switch(evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch(cmd)
      {
        case CMD_MODE: Mode_Change(); break;
        case CMD_ENTER: // KEY4进入设置打开时间
          Global_State = FSM_SET_OPEN_TIME;
          g_setting_hour = Sys_Context.openHour;
          Sys_Context.focus = FOCUS_OPEN_TIME;
          break;
        default: break;
      }
      break;
    case EVT_TIMER_CTRL:
      if(evt->param == 1)
      {
        if(Sys_Context.curtainState == CLOSED)
        {
          Sys_Context.curtainState = OPENING;
          Curtain_Open();
        }
      }
      else
      {
        if(Sys_Context.curtainState == OPENED)
        {
          Sys_Context.curtainState = CLOSING;
          Curtain_Close();
        }
      }
      break;
  }
}

void Handle_Idle_Manual(Event_t *evt)
{
  UserCMD_t cmd;
  switch (evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch (cmd)
      {
        case CMD_MODE:  Mode_Change();   break;
        case CMD_OPEN:
          if(Sys_Context.curtainState == CLOSED)
          {
            Curtain_Open();
          }
          break;
        case CMD_CLOSE:
          if(Sys_Context.curtainState == OPENED)
          {
            Curtain_Close();
          }
          break;
        case CMD_SET_TIME: Enter_SystemTime_Setting(); break;
        default: break;
      }
      break;
  }
}

void Handle_Opening(Event_t *evt)
{
  switch (evt->type)
  {
    case EVT_MOTOR_STATE:
      if(evt->param == 0)
      {
        Sys_Context.curtainState = OPENED;
        if(Sys_Context.mode == MODE_AUTO_LUX)
          Global_State = FSM_IDLE_LUX;
        else if(Sys_Context.mode == MODE_AUTO_TIM)
          Global_State = FSM_IDLE_TIM;
        else
          Global_State = FSM_IDLE_MANUAL;
      }
    break;
  }
}

void Handle_Closing(Event_t *evt)
{
  switch (evt->type)
  {
    case EVT_MOTOR_STATE:
      if(evt->param == 0)
      {
        Sys_Context.curtainState = CLOSED;
        if(Sys_Context.mode == MODE_AUTO_LUX)
          Global_State = FSM_IDLE_LUX;
        else if(Sys_Context.mode == MODE_AUTO_TIM)
          Global_State = FSM_IDLE_TIM;
        else
          Global_State = FSM_IDLE_MANUAL;
      }
    break;
  }
}

void Handle_SetOpenTime(Event_t *evt)
{
  UserCMD_t cmd;
  switch(evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch(cmd)
      {
        case CMD_INC:  // KEY2增加小时
          if(g_setting_hour < 23)
            g_setting_hour++;
          else
            g_setting_hour = 0;
          break;
        case CMD_DEC:  // KEY3减少小时
          if(g_setting_hour > 0)
            g_setting_hour--;
          else
            g_setting_hour = 23;
          break;
        case CMD_ENTER:  // KEY4确认，进入设置关闭时间
          Sys_Context.openHour = g_setting_hour;
          Global_State = FSM_SET_CLOSE_TIME;
          g_setting_hour = Sys_Context.closeHour;
          Sys_Context.focus = FOCUS_CLOSE_TIME;
          break;
        default: break;
      }
      break;
  }
}

void Handle_SetCloseTime(Event_t *evt)
{
  UserCMD_t cmd;
  switch(evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch(cmd)
      {
        case CMD_INC:  // KEY2增加小时
          if(g_setting_hour < 23)
            g_setting_hour++;
          else
            g_setting_hour = 0;
          break;
        case CMD_DEC:  // KEY3减少小时
          if(g_setting_hour > 0)
            g_setting_hour--;
          else
            g_setting_hour = 23;
          break;
        case CMD_ENTER:  // KEY4确认，返回空闲态
          Sys_Context.closeHour = g_setting_hour;
          Global_State = FSM_IDLE_TIM;
          Sys_Context.focus = FOCUS_NONE;
          break;
        default: break;
      }
      break;
  }
}

void Handle_SetSystemTime(Event_t *evt)
{
  UserCMD_t cmd;
  
  switch(evt->type)
  {
    case EVT_KEY_PRESS:
      cmd = Map_Key_To_Cmd(evt->param, Sys_Context.mode);
      switch(cmd)
      {
        case CMD_OPEN:
          if(Sys_Context.focus == FOCUS_HOUR)
          {
            if(g_temp_time.hour < 23)
              g_temp_time.hour++;
            else
              g_temp_time.hour = 0;
          }
          else if(Sys_Context.focus == FOCUS_MIN)
          {
            if(g_temp_time.min < 59)
              g_temp_time.min++;
            else
              g_temp_time.min = 0;
          }
          break;
          
        case CMD_CLOSE:
          if(Sys_Context.focus == FOCUS_HOUR)
          {
            if(g_temp_time.hour > 0)
              g_temp_time.hour--;
            else
              g_temp_time.hour = 23;
          }
          else if(Sys_Context.focus == FOCUS_MIN)
          {
            if(g_temp_time.min > 0)
              g_temp_time.min--;
            else
              g_temp_time.min = 59;
          }
          break;
          
        case CMD_SET_TIME:
          if(Sys_Context.focus == FOCUS_HOUR)
          {
            Sys_Context.focus = FOCUS_MIN;
          }
          else if(Sys_Context.focus == FOCUS_MIN)
          {
            SoftTime_Set(g_temp_time.hour, g_temp_time.min, 0);
            Global_State = FSM_IDLE_MANUAL;
            Sys_Context.focus = FOCUS_NONE;
            SoftTime_Get(&current_time);
            g_temp_time.hour = current_time.hour;
            g_temp_time.min = current_time.min;
          }
          break;
          
        default: break;
      }
      break;
  }
}

void Handle_Error(Event_t *evt)
{
  switch (evt->type)
  {
    case EVT_KEY_PRESS:
      if(Map_Key_To_Cmd(evt->param, Sys_Context.mode) == CMD_MODE)
      {
        Global_State = FSM_IDLE_TIM;
        Sys_Context.focus = FOCUS_NONE;
      }
      break;
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  
  /*初始化*/
  Event_Init();//初始化事件
  LED_Init();
  DHT11_Init();
  OLED_Init();
  Key_Init();
  Beep_Init();
  Light_Init();
  MQ7_Init();
  JR6001_Init();
  BT_Init();
  SoftTime_Init(0, 0, 0);
  Sys_Context.openHour = 8;    // 默认打开时间8点
  Sys_Context.closeHour = 18;  // 默认关闭时间18点
  SensorScan();

  /* 开启定时器2中断,频率1kHz */
  HAL_TIM_Base_Start_IT(&htim2);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    UI_Update();//刷新显示
    evt = Event_Dequeue();
    if(evt.type != EVT_NONE)
    {
      System_Dispatch(&evt);
    }

    HAL_IWDG_Refresh(&hiwdg);

    if (sensorscan_flag)
    {
      sensorscan_flag = 0;
      SensorScan();//扫描传感器数据
    }
    if (btprocess_flag)
    {
      btprocess_flag = 0;
      BT_Process();//处理蓝牙数据
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
 * @brief  定时器更新中断回调函数
 * @param  htim: 定时器句柄
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)//定时器更新中断回调函数
{
  /* 检查是否是TIM2触发的中断 */
  if (htim->Instance == TIM2)
  {
    SoftTime_Tick_1ms();//软件定时器1ms tick

    key_scan_tick++;
    if (key_scan_tick >= KEY_SCAN_INTERVAL)
    {
      key_scan_tick = 0;
      Key_Scan();
    }

    led_tick++;
    if (led_tick >= LED_TOGGLE_INTERVAL)
    {
      led_tick = 0;
      LED_Toggle();
    }

    time_ctrl_tick++;
    if (time_ctrl_tick >= SOFT_TIME_INTERVAL)
    {
      time_ctrl_tick = 0;
      SoftTime_CTRL();
    }

    sensor_tick++;
    if (sensor_tick >= SENSOR_SCAN_INTERVAL)
    {
      sensor_tick = 0;
      sensorscan_flag = 1;
    }

    bt_tick++;
    if (bt_tick >= BT_PROCESS_INTERVAL)
    {
      bt_tick = 0;
      btprocess_flag = 1;
    }

    MotorStep_Process();
    Beep_Process();
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
