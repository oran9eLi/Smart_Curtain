#ifndef __BSP_JR6001_H
#define __BSP_JR6001_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "usart.h"

/**
 * @brief JR6001语音模块初始化
 */
void JR6001_Init(void);

/**
 * @brief 播放指定ID的音频
 * @param id 音频ID (1-9999)
 */
void JR6001_Play(uint16_t id);

/**
 * @brief 设置音量
 * @param volume 音量等级 (0-30)
 */
void JR6001_SetVolume(uint8_t volume);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_JR6001_H */
