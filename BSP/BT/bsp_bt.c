#include "bsp_bt.h"
#include "event.h"
#include "soft_time.h"
#include <stdio.h>
#include <string.h>

volatile uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];
volatile uint8_t bt_rx_len = 0;
volatile uint8_t bt_rx_flag = 0;

static uint8_t temp_buffer[BT_RX_BUFFER_SIZE];

void BT_Init(void)
{
  bt_rx_len = 0;
  bt_rx_flag = 0;
  memset((void *)bt_rx_buffer, 0, BT_RX_BUFFER_SIZE);
  
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&huart2, (uint8_t *)bt_rx_buffer, BT_RX_BUFFER_SIZE);
}

void BT_IDLE_Handler(void)
{
  if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))
  {
    __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    
    HAL_UART_AbortReceive_IT(&huart2);// 取消接收中断
    
    bt_rx_len = BT_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
    bt_rx_flag = 1;
    
    HAL_UART_Receive_DMA(&huart2, (uint8_t *)bt_rx_buffer, BT_RX_BUFFER_SIZE);
  }
}

/**
 * @brief  处理蓝牙接收数据
 * @param  None
 * @retval None
 */
void BT_Process(void)
{
  if(bt_rx_flag)
  {
    bt_rx_flag = 0;
    
    memcpy(temp_buffer, (void *)bt_rx_buffer, bt_rx_len);
    temp_buffer[bt_rx_len] = '\0';
    
    BT_CmdPacket_t packet;
    if(BT_ParseCommand(temp_buffer, bt_rx_len, &packet))
    {
      Event_Queue(EVT_BT_CMD, (uint32_t)packet.cmd | ((uint32_t)packet.param[0] << 8) | 
                  ((uint32_t)packet.param[1] << 16) | ((uint32_t)packet.param[2] << 24));
    }
    
    memset((void *)bt_rx_buffer, 0, BT_RX_BUFFER_SIZE);
    bt_rx_len = 0;
  }
}

/**
 * @brief  解析蓝牙命令包
 * @param  data 包含命令的字节数组
 * @param  len 数据长度
 * @param  packet 指向命令包结构体的指针
 * @retval 是否成功解析
 */
bool BT_ParseCommand(uint8_t *data, uint8_t len, BT_CmdPacket_t *packet)
{
  if(len < 2) return false;
  if(!BT_IsValidCommand(data, len)) return false;
  
  packet->cmd = BT_CMD_NONE;
  packet->param_len = 0;
  memset(packet->param, 0, 4);
  
  switch(data[0])
  {
    case 'O':
      if(data[1] == 'P')
        packet->cmd = BT_CMD_OPEN;
      break;
        
    case 'C':
      if(data[1] == 'L')
        packet->cmd = BT_CMD_CLOSE;
      else if(data[1] == 'S')
        packet->cmd = BT_CMD_SET_TIME;
      break;
        
    case 'G':
      if(data[1] == 'S')
        packet->cmd = BT_CMD_GET_STATUS;
      break;
        
    case 'M':
      if(data[1] == 'O')
        packet->cmd = BT_CMD_SWITCH_MODE;
      break;
        
    case 'T':
      if(len >= 4)
      {
        if(data[1] == 'O')
        {
          packet->cmd = BT_CMD_SET_OPEN_TIME;
          uint8_t hour = (data[2] - '0') * 10 + (data[3] - '0');
          
          // 验证小时范围 0-23
          if(hour > 23)
          {
            // 无效时间，不设置命令
            packet->cmd = BT_CMD_NONE;
          }
          else
          {
            packet->param[0] = hour;
            packet->param_len = 1;
          }
        }
        else if(data[1] == 'C')
        {
          packet->cmd = BT_CMD_SET_CLOSE_TIME;
          uint8_t hour = (data[2] - '0') * 10 + (data[3] - '0');
          
          // 验证小时范围 0-23
          if(hour > 23)
          {
            // 无效时间，不设置命令
            packet->cmd = BT_CMD_NONE;
          }
          else
          {
            packet->param[0] = hour;
            packet->param_len = 1;
          }
        }
        else if(data[1] == 'S')
        {
          packet->cmd = BT_CMD_SET_TIME;
          uint8_t hour   = (data[2] - '0') * 10 + (data[3] - '0');
          uint8_t minute = (data[4] - '0') * 10 + (data[5] - '0');
          uint8_t second = (data[6] - '0') * 10 + (data[7] - '0');
          
          // 验证时间有效性
          if(hour > 23 || minute > 59 || second > 59)
          {
            packet->cmd = BT_CMD_NONE;
          }
          else
          {
            packet->param[0] = hour;
            packet->param[1] = minute;
            packet->param[2] = second;
            packet->param_len = 3;
          }
        }
      }
      break;
    default:
      break;
  }
  return (packet->cmd != BT_CMD_NONE);
}
/**
 * @brief  执行蓝牙命令
 * @param  packet 指向命令包结构体的指针
 */
void BT_ExecuteCommand(BT_CmdPacket_t *packet)
{
  if(packet == NULL) return;
  
  SoftTime_t current_time;
  char response[32];
  
  switch(packet->cmd)
  {
    case BT_CMD_OPEN:
      if(Sys_Context.curtainState == CLOSED)
      {
        BT_SendResponse("OK:OPEN");
        Curtain_Open();
      }
      else
      {
        BT_SendResponse("ERR:ALREADY_OPEN");
      }
      break;
        
    case BT_CMD_CLOSE:
      if(Sys_Context.curtainState == OPENED)
      {
        BT_SendResponse("OK:CLOSE");
        Curtain_Close();
      }
      else
      {
        BT_SendResponse("ERR:ALREADY_CLOSED");
      }
      break;
        
    case BT_CMD_GET_STATUS:
      SoftTime_Get(&current_time);
      snprintf(response, sizeof(response), "TIM%02d%02d%02d CURS:%d MODE:%d LUX:%d",
                current_time.hour, current_time.min, current_time.sec,
                Sys_Context.curtainState, Sys_Context.mode, Sys_Context.luxState);
      BT_SendResponse(response);
      break;
        
    case BT_CMD_SWITCH_MODE:
      BT_SendResponse("OK:MODE");
      Mode_Change();
      break;
        
    case BT_CMD_SET_OPEN_TIME:
    {
      uint8_t hour = packet->param[0];
      if(hour > 23)
      {
        BT_SendResponse("ERR:INVALID_HOUR");
        break;
      }
      Sys_Context.openHour = hour;
      snprintf(response, sizeof(response), "OK:TO%02d", packet->param[0]);
      BT_SendResponse(response);
      break;
    }
        
    case BT_CMD_SET_CLOSE_TIME:
    {
      uint8_t hour = packet->param[0];
      if(hour > 23)
      {
        BT_SendResponse("ERR:INVALID_HOUR");
        break;
      }
      Sys_Context.closeHour = hour;
      snprintf(response, sizeof(response), "OK:TC%02d", packet->param[0]);
      BT_SendResponse(response);
      break;
    }    
    case BT_CMD_SET_TIME:
    {
      uint8_t hour = packet->param[0];
      uint8_t minute = packet->param[1];
      uint8_t second = packet->param[2];
      if(hour > 23 || minute > 59 || second > 59)
      {
          BT_SendResponse("ERR:INVALID_TIME");
          break;
      }
      SoftTime_Set(hour, minute, second);
      snprintf(response, sizeof(response), "OK:TS%02d%02d%02d",
                packet->param[0], packet->param[1], packet->param[2]);
      BT_SendResponse(response);
      break;
    }    
    default:
      BT_SendResponse("ERR:UNKNOWN");
      break;
  }
}

void BT_SendResponse(const char *response)
{
    if(response == NULL) return;
    HAL_UART_Transmit(&huart2, (uint8_t *)response, strlen(response), 100);
    HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
}

/**
 * @brief  验证蓝牙命令是否有效
 * @param  data 包含命令的字节数组
 * @param  len 数据长度
 * @retval 是否有效
 */
bool BT_IsValidCommand(uint8_t *data, uint8_t len)
{
    if(len < 2) return false;
    
    for(uint8_t i = 0; i < len; i++)
    {
        if(data[i] >= 'a' && data[i] <= 'z')
            data[i] = data[i] - 'a' + 'A';
    }
    
    return true;
}

/**
 * @brief  将十六进制字符转换为十进制数值
 * @param  high 高 nibble 字符
 * @param  low 低 nibble 字符
 * @retval 转换后的数值
 */
uint8_t BT_HexToDec(uint8_t high, uint8_t low)
{
    uint8_t result = 0;
    
    if(high >= '0' && high <= '9')
        result = (high - '0') << 4;
    else if(high >= 'A' && high <= 'F')
        result = (high - 'A' + 10) << 4;
    
    if(low >= '0' && low <= '9')
        result |= (low - '0');
    else if(low >= 'A' && low <= 'F')
        result |= (low - 'A' + 10);
    
    return result;
}
