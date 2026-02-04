#include "bsp_motor.h"


/**
 * @brief  步进电机状态表
 * @note   每个状态对应电机的一个状态，用于控制电机的旋转方向和角度
 * @retval None
 */
static const GPIO_PinState Step_Table[8][4] = {
  {GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET},
  {GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET},
  {GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET},
  {GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET},
  {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET},
  {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET},
  {GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET},
  {GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET}
};

static volatile uint8_t step_index = 0;//当前步进索引
static volatile uint8_t motor_run = 0;//电机运行状态
static volatile uint16_t step_tick = 0;//当前步进时间
static volatile uint16_t step_period = 2;//步进周期 2ms (500Hz)
static volatile uint32_t steps_to_run = 0;//剩余步数
static volatile Motor_Dir_t current_dir = MOTOR_STOP_MODE;//当前电机方向


/**
 * @brief  设置电机引脚状态
 * @note   根据当前步进索引设置电机的引脚状态，实现电机的旋转
 * @param  index: 步进索引，范围0~7
 * @retval None
 */
static void Motor_Set_Pins(uint8_t index)
{
  HAL_GPIO_WritePin(MOTOR_AB_PORT, MOTOR_A, Step_Table[index][0]);
  HAL_GPIO_WritePin(MOTOR_AB_PORT, MOTOR_B, Step_Table[index][1]);
  HAL_GPIO_WritePin(MOTOR_CD_PORT, MOTOR_C, Step_Table[index][2]);
  HAL_GPIO_WritePin(MOTOR_CD_PORT, MOTOR_D, Step_Table[index][3]);
}

/**
 * @brief  启动电机
 * @note   根据指定方向启动电机，若方向为MOTOR_STOP_MODE，则停止电机
 * @param  dir: 电机方向，MOTOR_CW或MOTOR_CCW
 * @retval None
 */
void Motor_Start(Motor_Dir_t dir)
{
  if (dir == MOTOR_STOP_MODE)
  {
    Motor_Stop();
    return;
  }

  current_dir = dir;
  motor_run = 1;
}

/**
 * @brief  旋转电机指定角度
 * @note   根据指定方向和角度启动电机，若角度为0，则停止电机
 * @param  dir: 电机方向，MOTOR_CW或MOTOR_CCW
 * @param  angle: 旋转角度，范围0~360
 * @retval None
 */
void Motor_Rotate_Angle(Motor_Dir_t dir, uint16_t angle)
{
  uint32_t total_steps = ((uint32_t)angle * 4096u) / 360u;//计算总步数

  if (total_steps == 0)
  {
    Motor_Stop();
    return;
  }

  steps_to_run = total_steps;
  current_dir = dir;
  motor_run = 1;
  Event_Queue(EVT_MOTOR_STATE, motor_run);//入队事件
}

/**
 * @brief  停止电机
 * @note   停止电机运行，将引脚状态设置为低电平
 * @retval None
 */
void Motor_Stop(void)
{
  motor_run = 0;
  steps_to_run = 0;
  current_dir = MOTOR_STOP_MODE;
  HAL_GPIO_WritePin(MOTOR_AB_PORT, MOTOR_A | MOTOR_B, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MOTOR_CD_PORT, MOTOR_C | MOTOR_D, GPIO_PIN_RESET);
  Event_Queue(EVT_MOTOR_STATE, motor_run);//入队事件
}

/**
 * @brief  电机步进处理函数
 * @note   用于处理电机的步进操作，根据当前运行状态和方向进行引脚状态设置
 * @retval None
 */
void MotorStep_Process(void)
{
  if (motor_run == 0)
  {
    return;
  }

  step_tick++;
  if (step_tick < step_period)
  {
    return;
  }
  step_tick = 0;

  if (current_dir == MOTOR_CW)
  {
    step_index = (uint8_t)((step_index + 1) & 0x07);
  }
  else if (current_dir == MOTOR_CCW)
  {
    step_index = (uint8_t)((step_index + 7) & 0x07);
  }
  else
  {
    return;
  }

  Motor_Set_Pins(step_index);

  if (steps_to_run > 0)
  {
    steps_to_run--;
    if (steps_to_run == 0)
    {
      Motor_Stop();
    }
  }
}
