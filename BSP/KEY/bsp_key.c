#include "bsp_key.h"

static Key_t key_state[KEY_COUNT];
static const GPIO_TypeDef* key_ports[KEY_COUNT] = {KEY1_GPIO_Port, KEY2_GPIO_Port, KEY3_GPIO_Port, KEY4_GPIO_Port};
static const uint16_t key_pins[KEY_COUNT] = {KEY1_Pin, KEY2_Pin, KEY3_Pin, KEY4_Pin};

/**
 * @brief 初始化按键
 * @retval None
 */
void Key_Init(void)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    key_state[i].state = KEY_STATE_IDLE;
    key_state[i].counter = 0;
  }
}

/**
 * @brief 获取按键电平
 * @param key_id 按键ID
 * @retval 按键电平(0/1)
 */
uint8_t Key_GetLevel(Key_ID_t key_id)
{
  if (key_id >= KEY_COUNT)
  {
    return 1;
  }
  return HAL_GPIO_ReadPin((GPIO_TypeDef*)key_ports[key_id], key_pins[key_id]);
}

/**
 * @brief 扫描按键
 * @retval None
 */
void Key_Scan(void)
{
  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    uint8_t level = Key_GetLevel((Key_ID_t)i);

    switch (key_state[i].state)
    {
      case KEY_STATE_IDLE:
        if (level == 0)
        {
          key_state[i].state = KEY_STATE_DEBOUNCE;
          key_state[i].counter = 0;
        }
        break;

      case KEY_STATE_DEBOUNCE:
        if (level == 0)
        {
          key_state[i].counter++;
          if (key_state[i].counter >= KEY_DEBOUNCE_COUNTER)
          {
            key_state[i].state = KEY_STATE_PRESSED;
          }
        }
        else
        {
          key_state[i].state = KEY_STATE_IDLE;
        }
        break;

      case KEY_STATE_PRESSED:
        if (level == 1)
        {
          Event_Queue(EVT_KEY_PRESS, i);//入队事件
          key_state[i].state = KEY_STATE_IDLE;
        }
        break;

      default:
        key_state[i].state = KEY_STATE_IDLE;
        break;
    }
  }
}
