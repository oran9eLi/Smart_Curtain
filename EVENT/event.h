#ifndef __EVENT_H
#define __EVENT_H

#include "stdint.h"
#include "stdbool.h"

typedef enum {
	EVT_NONE = 0,//无事件
	EVT_KEY_PRESS,//按键事件(参数：按键代码)
	EVT_BT_CMD,//蓝牙命令事件(参数：蓝牙命令代码)
	EVT_SENSOR_LIGHT,//光照传感器事件(参数：0/1阈值)
	EVT_SENSOR_CO,//CO传感器事件(参数：0/1阈值)
  EVT_MOTOR_STATE,//电机状态事件(参数：0/1关闭/开启)
  EVT_TIMER_CTRL,//定时器控制事件(参数：0/1关闭/开启)
	EVT_ERROR//错误事件(参数：错误类型)
} Event_Type;

typedef struct
{
  Event_Type type;//事件类型
  uint32_t param;//事件参数
} Event_t;

void Event_Init(void);//初始化事件
void Event_Queue(Event_Type type, uint32_t param);//入队事件
Event_t Event_Dequeue(void);//出队事件


#endif
