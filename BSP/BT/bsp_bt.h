#ifndef __BSP_BT_H
#define __BSP_BT_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "soft_time.h"
#include <stdint.h>
#include <stdbool.h>

/* 蓝牙接收缓冲区大小 */
#define BT_RX_BUFFER_SIZE 64

/* 蓝牙命令定义 */
typedef enum {
  BT_CMD_NONE = 0,
  BT_CMD_OPEN,        // 打开窗帘
  BT_CMD_CLOSE,       // 关闭窗帘
  BT_CMD_SET_TIME,    // 设置系统时间
  BT_CMD_SET_OPEN_TIME,   // 设置打开时间
  BT_CMD_SET_CLOSE_TIME,  // 设置关闭时间
  BT_CMD_GET_STATUS,  // 获取状态
  BT_CMD_SWITCH_MODE, // 切换工作模式
} BT_Command_t;

/* 蓝牙命令结构体 */
typedef struct {
  BT_Command_t cmd;
  uint8_t param[4];   // 参数数组，用于时间设置等
  uint8_t param_len;
} BT_CmdPacket_t;

/* 外部变量声明 */
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern SysStatus_t Sys_Context;
extern volatile uint8_t bt_rx_buffer[BT_RX_BUFFER_SIZE];
extern volatile uint8_t bt_rx_len;
extern volatile uint8_t bt_rx_flag;

void Mode_Change(void);
void Curtain_Open(void);
void Curtain_Close(void);
/* 函数声明 */
void BT_Init(void);
void BT_Process(void);
bool BT_ParseCommand(uint8_t *data, uint8_t len, BT_CmdPacket_t *packet);
void BT_ExecuteCommand(BT_CmdPacket_t *packet);
void BT_SendResponse(const char *response);
void BT_IDLE_Handler(void);

/* 协议解析辅助函数 */
bool BT_IsValidCommand(uint8_t *data, uint8_t len);

#endif /* __BSP_BT_H */
