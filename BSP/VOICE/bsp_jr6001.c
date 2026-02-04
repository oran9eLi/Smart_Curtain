#include "bsp_jr6001.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief JR6001语音模块初始化
 * @note 目前主要是串口初始化，已经在 MX_USART3_UART_Init 中完成
 */
void JR6001_Init(void)
{
    // 初始化时将音量设置为最大 (30)
    JR6001_SetVolume(30);
}

/**
 * @brief 播放指定ID的音频
 * @param id 音频ID (1-9999)
 * @note 发送格式为 A7:0001, A7:0002 等
 */
void JR6001_Play(uint16_t id)
{
    char cmd[16];
    // 格式化指令，例如 id=1 变为 A7:00001
    // 注意：根据用户要求使用 A7:0001 格式
    sprintf(cmd, "A7:%05d\r\n", id); 
    
    // 通过串口3发送指令
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
}

/**
 * @brief 设置音量
 * @param volume 音量等级 (0-30)
 * @note 发送格式为 AF:30
 */
void JR6001_SetVolume(uint8_t volume)
{
    char cmd[16];
    if (volume > 30) volume = 30;
    
    // 格式化音量指令
    sprintf(cmd, "AF:%02d\r\n", volume);
    
    // 通过串口3发送指令
    HAL_UART_Transmit(&huart3, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
}
