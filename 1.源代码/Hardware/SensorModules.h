#ifndef	__SENSORMODULES_H_
#define __SENSORMODULES_H_

#include "stm32f10x.h"                  // Device header


typedef struct
{
	uint8_t humi;
	uint8_t temp;
	uint8_t people;
	uint8_t flame;	
	uint16_t smog;	
	uint16_t methane;	
}SensorModules;

typedef struct
{
	uint8_t tempValue;
	uint8_t flameValue;
	uint16_t smogValue;	
	uint16_t methaneValue;	
	
}SensorThresholdValue;

typedef struct
{
	uint8_t led;
	uint8_t waterPump;
	uint8_t smogAlarm;
	uint8_t methaneAlarm;
	uint8_t flameAlarm;	
} SystemState;

extern SystemState	systemState;			//声明系统状态结构体变量
extern SensorModules sensorData;			//声明传感器模块的结构体变量
extern SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量

#endif
