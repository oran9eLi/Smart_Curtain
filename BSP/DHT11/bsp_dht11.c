#include "bsp_dht11.h"

/* 声明系统时钟频率变量 */
static uint32_t DHT11_CPU_Freq_MHz = 72; // 默认为72MHz，Init中会自动更新

/**
 * @brief  微秒级延时 (基于DWT)
 * @param  us: 延时微秒数
 */
void delay_us(uint32_t us)
{
  uint32_t start_tick = DWT->CYCCNT;
  uint32_t delay_ticks = us * DHT11_CPU_Freq_MHz;

  /* 等待时间到达 */
  while ((DWT->CYCCNT - start_tick) < delay_ticks)
      ;
}

/**
 * @brief  配置引脚为推挽输出模式
 */
static void DHT11_Mode_Out_PP(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  配置引脚为输入模式 (带上拉)
 */
static void DHT11_Mode_In(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  读取一个字节数据
 * @return 读取到的字节
 */
static uint8_t DHT11_Read_Byte(void)
{
  uint8_t i, data = 0;
  uint32_t retry = 0;

  for (i = 0; i < 8; i++)
  {
    /* 每个bit以50us低电平开始 */
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
    {
        if (++retry > 10000)
            return 0; // 超时防止死循环
    }

    /* 随后是高电平，26-28us表示'0'，70us表示'1' */
    delay_us(40); // 延时40us后检测电平

    if (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
    {
      data |= (1 << (7 - i)); // 高位在前
      /* 等待高电平结束 */
      retry = 0;
      while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
      {
        if (++retry > 10000)
          break; // 超时
      }
    }
  }
  return data;
}

/**
 * @brief  DHT11初始化
 *         注意：需要在 main.c 中调用此函数
 */
void DHT11_Init(void)
{
  /* 获取当前HCLK频率 (MHz) */
  DHT11_CPU_Freq_MHz = HAL_RCC_GetHCLKFreq() / 1000000;

  /* 开启DWT计数器用于微秒延时 */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* 初始状态：输出高电平 */
  DHT11_Mode_Out_PP();
  HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
  HAL_Delay(1000); // 上电后等待1s让DHT11稳定
}

/**
 * @brief  读取温湿度数据
 * @param  temp: 温度值指针 (0-50°C)
 * @param  humi: 湿度值指针 (20-90%RH)
 * @return 0: 成功, 1: 失败
 */
uint8_t DHT11_Read_Data(float *temp, float *humi)
{
  uint8_t buf[5];
  uint8_t i;

  /* 1. 主机发送开始信号 */
  DHT11_Mode_Out_PP();
  HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);
  HAL_Delay(20); // 拉低至少18ms
  HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET);
  delay_us(30); // 拉高20~40us

  /* 2. 主机设为输入，等待DHT11响应 */
  DHT11_Mode_In();

  /* 等待DHT11拉低 (响应信号) */
  if (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
  {
    uint32_t retry = 0;

    /* 等待DHT11响应的低电平结束 (80us) */
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_RESET)
    {
      if (++retry > 10000)
        return 1;
    }

    /* 等待DHT11响应的高电平结束 (80us) */
    retry = 0;
    while (HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin) == GPIO_PIN_SET)
    {
      if (++retry > 10000)
        return 1;
    }

    /* 3. 开始读取40位数据 */
    for (i = 0; i < 5; i++)
    {
      buf[i] = DHT11_Read_Byte();
    }

    /* 4. 校验数据 */
    /* 校验和 = 湿度整数 + 湿度小数 + 温度整数 + 温度小数 */
    if (buf[4] == (buf[0] + buf[1] + buf[2] + buf[3]))
    {
      *humi = buf[0] + buf[1] / 10.0f; // 湿度
      *temp = buf[2] + buf[3] / 10.0f; // 温度
      return 0;                        // 成功
    }
  }

  return 1; // 失败
}
