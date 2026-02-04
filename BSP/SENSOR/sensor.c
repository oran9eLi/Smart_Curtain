#include "sensor.h"

/**
 * @brief  扫描传感器数据
 * @param  无
 * @retval 无
 */
void SensorScan(void)
{
  // 扫描DHT11
  DHT11_Read_Data(&sensor_data.temp, &sensor_data.humi);
  // 扫描光敏传感器
  sensor_data.lux = (uint16_t)Light_Read_Percent();
  // 扫描MQ-7 CO传感器
  sensor_data.CO = (uint16_t)MQ7_Read_Percent();


  //数据处理
  if(sensor_data.lux < 45)
  {
    Event_Queue(EVT_SENSOR_LIGHT, 0);//入队事件
  }
  else if(sensor_data.lux > 55)
  {
    Event_Queue(EVT_SENSOR_LIGHT, 1);//入队事件
  }
  
  if(sensor_data.CO > 30)
  {
    Event_Queue(EVT_SENSOR_CO, 1);//入队事件
  }
  else if(sensor_data.CO < 20)
  {
    Event_Queue(EVT_SENSOR_CO, 0);//入队事件
  }
}
