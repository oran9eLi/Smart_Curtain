#ifndef __SENSOR_H
#define __SENSOR_H

#include "main.h"
#include "event.h"
#include "bsp_dht11.h"
#include "bsp_light.h"
#include "bsp_mq7.h"

#define SENSOR_SCAN_INTERVAL 1000//传感器扫描间隔1000ms

typedef struct
{
  float temp;
  float humi;
  uint16_t lux;
  uint16_t CO;
} SensorData_t;

extern SensorData_t sensor_data;

void SensorScan(void);

#endif
