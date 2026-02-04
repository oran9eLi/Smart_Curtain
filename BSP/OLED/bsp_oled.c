#include "bsp_oled.h"
#include "i2c.h"
#include "oledfont.h"

/* 显存: 8页 x 128列 */
static uint8_t OLED_GRAM[8][128];

/**
 * @brief  向OLED写入一个字节
 * @param  dat: 要写入的数据/命令
 * @param  cmd: 数据/命令标志 0-命令 1-数据
 */
void OLED_WriteByte(uint8_t dat, uint8_t cmd)
{
    // 寄存器地址：0x00为命令，0x40为数据
    uint8_t memAddr = (cmd == OLED_CMD) ? 0x00 : 0x40;
    HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, memAddr, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
}

/**
 * @brief  更新显存到OLED
 */
void OLED_Refresh(void)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_WriteByte(0xb0 + i, OLED_CMD); // 设置页地址（0~7）
        OLED_WriteByte(0x00, OLED_CMD);     // 设置显示位置—列低地址
        OLED_WriteByte(0x10, OLED_CMD);     // 设置显示位置—列高地址
        
        // I2C 突发写入一整页 (128字节)
        HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, 0x40, I2C_MEMADD_SIZE_8BIT, OLED_GRAM[i], 128, 100);
    }
}

/**
 * @brief  清屏
 */
void OLED_Clear(void)
{
    uint8_t i, n;
    for (i = 0; i < 8; i++)
    {
        for (n = 0; n < 128; n++)
        {
            OLED_GRAM[i][n] = 0;
        }
    }
    OLED_Refresh();
}

/**
 * @brief  画点
 * @param  x: 0~127
 * @param  y: 0~63
 * @param  t: 1-亮 0-灭
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t pos, bx, temp = 0;
    if (x > 127 || y > 63) return;
    
    pos = y / 8; // 页地址
    bx = y % 8;  // 页内位移
    temp = 1 << bx;
    
    if (t)
        OLED_GRAM[pos][x] |= temp;
    else
        OLED_GRAM[pos][x] &= ~temp;
}

/**
 * @brief  开启OLED显示
 */
void OLED_Display_On(void)
{
    OLED_WriteByte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WriteByte(0X14, OLED_CMD); // DCDC ON
    OLED_WriteByte(0XAF, OLED_CMD); // DISPLAY ON
}

/**
 * @brief  关闭OLED显示
 */
void OLED_Display_Off(void)
{
    OLED_WriteByte(0X8D, OLED_CMD); // SET DCDC命令
    OLED_WriteByte(0X10, OLED_CMD); // DCDC OFF
    OLED_WriteByte(0XAE, OLED_CMD); // DISPLAY OFF
}

/**
 * @brief  显示字符 (直接操作显存，优化版)
 * @param  x,y: 起始坐标
 * @param  chr: 字符
 * @param  Char_Size: 字体大小 12/16/6
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    unsigned char c = 0;
    uint8_t i = 0;
    
    c = chr - ' '; // 得到偏移后的值
    
    if (x > OLED_WIDTH - 1) { x = 0; y = y + 2; }
    
    if (Char_Size == 16) // 8x16
    {
        for (i = 0; i < 8; i++)
        {
            OLED_GRAM[y / 8][x + i] = asc2_1608[c][i];
            OLED_GRAM[y / 8 + 1][x + i] = asc2_1608[c][i + 8];
        }
    }
    else if (Char_Size == 12) // 6x12
    {
        for (i = 0; i < 6; i++)
        {
            OLED_GRAM[y / 8][x + i] = asc2_1206[c][i];
            OLED_GRAM[y / 8 + 1][x + i] = asc2_1206[c][i + 6];
        }
    }
    else if (Char_Size == 6) // 6x8
    {
        for (i = 0; i < 6; i++)
        {
            OLED_GRAM[y / 8][x + i] = asc2_0806[c][i];
        }
    }
}

/**
 * @brief  显示汉字 (16x16)
 * @param  x: X坐标 (0~112)
 * @param  y: Y坐标 (0~48)
 * @param  no: 汉字在Hzk1数组中的索引
 */
void OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t no)
{
    uint8_t i;
    // 边界检查
    if(x > 112 || y > 48) return;
    
    for(i = 0; i < 16; i++)
    {
        OLED_GRAM[y/8][x + i] = Hzk1[no][i];        // 上半部分 (0-15字节)
        OLED_GRAM[y/8 + 1][x + i] = Hzk1[no][i+16]; // 下半部分 (16-31字节)
    }
}

/**
 * @brief  显示字符串
 */
void OLED_ShowString(uint8_t x, uint8_t y, char *p, uint8_t Char_Size)
{
    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符
    {
        if (x > (128 - (Char_Size / 2))) { x = 0; y += Char_Size; }
        if (y > (64 - Char_Size)) { y = x = 0; OLED_Clear(); }
        OLED_ShowChar(x, y, *p, Char_Size);
        x += (Char_Size == 12 || Char_Size == 6) ? 6 : (Char_Size / 2); // 16->8, 12->6, 6->6
        p++;
    }
}

/**
 * @brief  计算m^n (内部使用)
 */
static uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--) result *= m;
    return result;
}

/**
 * @brief  显示数字
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    uint8_t char_width = (size == 12 || size == 6) ? 6 : (size / 2);

    for (t = 0; t < len; t++)
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                OLED_ShowChar(x + char_width * t, y, '0', size); // 补0
                // 如果想补空格: OLED_ShowChar(..., ' ', ...); 
                // 这里选择显示0，如 00123
                continue;
            }
            else
                enshow = 1;
        }
        OLED_ShowChar(x + char_width * t, y, temp + '0', size);
    }
}

/**
 * @brief  OLED初始化
 */
void OLED_Init(void)
{
    HAL_Delay(200); // 等待OLED上电稳定

    OLED_WriteByte(0xAE, OLED_CMD); // 关闭显示

    OLED_WriteByte(0x20, OLED_CMD); // 设置寻址模式 (0x00/0x01/0x02)
    OLED_WriteByte(0x02, OLED_CMD); // 页寻址模式

    OLED_WriteByte(0x00, OLED_CMD); // 设置列低地址
    OLED_WriteByte(0x10, OLED_CMD); // 设置列高地址

    OLED_WriteByte(0x40, OLED_CMD); // 设置起始行 (0x00~0x3F)

    OLED_WriteByte(0x81, OLED_CMD); // 对比度设置
    OLED_WriteByte(0xCF, OLED_CMD); // 输出电流亮度

    OLED_WriteByte(0xA1, OLED_CMD); // 设置段重映射 (0xA0:左右反置, 0xA1:正常)
    OLED_WriteByte(0xC8, OLED_CMD); // 设置行扫描方向 (0xC0:上下反置, 0xC8:正常)

    OLED_WriteByte(0xA6, OLED_CMD); // 正常/反相显示 (0xA6:正常, 0xA7:反相)

    OLED_WriteByte(0xA8, OLED_CMD); // 设置多路复用比 (1~64)
    OLED_WriteByte(0x3f, OLED_CMD); // 1/64 duty

    OLED_WriteByte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WriteByte(0x00, OLED_CMD); // 无偏移

    OLED_WriteByte(0xD5, OLED_CMD); // 设置显示时钟分频
    OLED_WriteByte(0x80, OLED_CMD); // 默认

    OLED_WriteByte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WriteByte(0xF1, OLED_CMD); // Pre-Charge 15, Discharge 1

    OLED_WriteByte(0xDA, OLED_CMD); // 设置COM硬件配置
    OLED_WriteByte(0x12, OLED_CMD);

    OLED_WriteByte(0xDB, OLED_CMD); // 设置VCOMH
    OLED_WriteByte(0x40, OLED_CMD);

    OLED_WriteByte(0x8D, OLED_CMD); // 电荷泵设置
    OLED_WriteByte(0x14, OLED_CMD); // 开启电荷泵

    OLED_WriteByte(0xA4, OLED_CMD); // 全局显示开启 (0xA4:正常, 0xA5:全亮)
    
    OLED_Clear();                   // 清屏
    
    OLED_WriteByte(0xAF, OLED_CMD); // 打开显示
}
