#ifndef __BSP_OLED_H
#define __BSP_OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ==============================================================================
 * OLED 参数定义 (SSD1306)
 * ==============================================================================*/
#define OLED_ADDRESS    0x78    // OLED I2C地址 (写地址)
                                // 如果是0x7A，请修改此处

/* 显存尺寸 */
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

/* 命令/数据 标志 */
#define OLED_CMD        0x00
#define OLED_DATA       0x40

/* ==============================================================================
 * 函数声明
 * ==============================================================================*/

/**
 * @brief  OLED初始化函数
 * @note   使用前请确保I2C总线已初始化
 */
void OLED_Init(void);

/**
 * @brief  OLED清屏
 */
void OLED_Clear(void);

/**
 * @brief  更新显存到OLED
 */
void OLED_Refresh(void);

/**
 * @brief  开启OLED显示
 */
void OLED_Display_On(void);

/**
 * @brief  关闭OLED显示
 */
void OLED_Display_Off(void);

/**
 * @brief  在指定位置显示一个字符
 * @param  x: X坐标 (0~127)
 * @param  y: Y坐标 (0~63)
 * @param  chr: 要显示的字符
 * @param  Char_Size: 字体大小 (12/16)
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size);

/**
 * @brief  显示汉字 (16x16)
 * @param  x: X坐标
 * @param  y: Y坐标
 * @param  no: 汉字在Hzk1数组中的索引
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no);

/**
 * @brief  显示字符串
 * @param  x: X坐标
 * @param  y: Y坐标
 * @param  p: 字符串指针
 * @param  Char_Size: 字体大小
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *p, uint8_t Char_Size);

/**
 * @brief  显示数字
 * @param  x: X坐标
 * @param  y: Y坐标
 * @param  num: 数字
 * @param  len: 数字长度
 * @param  size: 字体大小
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size);

/**
 * @brief  画点
 * @param  x: X坐标
 * @param  y: Y坐标
 * @param  t: 1-填充, 0-清空
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_OLED_H */
