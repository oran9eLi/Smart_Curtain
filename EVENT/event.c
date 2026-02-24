#include "event.h"
#include "main.h"

#define EVENT_QUEUE_SIZE 10//事件队列大小

static Event_t eventQueue[EVENT_QUEUE_SIZE];//事件队列
static uint32_t head = 0;//队头指针
static uint32_t tail = 0;//队尾指针
static uint32_t count = 0;//队列中事件数量


/**
 * @brief 初始化事件
 * @retval None
 */
void Event_Init(void)//初始化事件
{
  head = 0;//队头指针指向0
  tail = 0;//队尾指针指向0
  count = 0;//队列中事件数量为0
}

/**
 * @brief 入队事件
 * @param evt 指向事件结构体的指针
 * @retval None
 */
void Event_Queue(Event_Type type, uint32_t param)//入队事件
{
  if(count < EVENT_QUEUE_SIZE)//队列未满
  {
    eventQueue[tail].type = type;//将事件类型入队
    eventQueue[tail].param = param;//将事件参数入队
    tail = (tail + 1) % EVENT_QUEUE_SIZE;//更新队尾指针
    count++;//队列中事件数量加1
  }
}

/**
 * @brief 出队事件
 * @param None
 * @retval 出队事件
 */
Event_t Event_Dequeue(void)//出队事件
{
  Event_t evt;//定义事件变量
  evt.type = EVT_NONE;//将事件类型设为无事件
  evt.param = 0;//将事件参数设为0
  if(count > 0)//队列不为空
  {
    evt.type = eventQueue[head].type;//将队头事件类型出队
    evt.param = eventQueue[head].param;//将队头事件参数出队
    head = (head + 1) % EVENT_QUEUE_SIZE;//更新队头指针
    count--;//队列中事件数量减1
  }
  return evt;//返回出队事件
}
