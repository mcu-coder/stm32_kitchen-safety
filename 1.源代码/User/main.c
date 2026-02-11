#include "stm32f10x.h"                  // Device header
#include "iwdg.h"
#include "adcx.h"
#include "oled.h"
#include "dht11.h"
#include "led.h"
#include "key.h"
#include "tim2.h"   
#include "tim3.h"   
#include "usart3.h"   
#include "usart.h"
#include "sensormodules.h"
#include "gizwits_product.h"
#include "flash.h"
#include "hc_sr501.h"
#include "servo.h"
#include "pwm.h"

#define KEY_1	1
#define KEY_2	2
#define KEY_3	3
#define KEY_4	4

#define FLASH_START_ADDR	0x0801f000	//写入的起始地址

SystemState	systemState;			//声明系统状态结构体变量
SensorModules sensorData;	//声明传感器数据结构体变量
SensorThresholdValue Sensorthreshold;	//声明传感器阈值结构体变量

uint8_t menu = 1;	//显示菜单变量
uint8_t OLED_Clear_Flag;	//阈值设置界面的清屏标志位
uint8_t mode = 0;	//系统模式

uint8_t displayPageNum;	//主页面显示页面

uint8_t valveLockFlag;	//阀门自锁标志位

enum 
{
	display_page1 = 1,
	display_page2,
	settingsPage
	
}MenuPages;

/**
  * @brief  显示菜单1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_Menu1(void)
{
 
	
	//显示“一氧化碳： ”
	OLED_ShowChinese(3, 1, 42);
	OLED_ShowChinese(3, 2, 43);
	OLED_ShowChinese(3, 3, 44);
	OLED_ShowChinese(3, 4, 45);
	OLED_ShowChar(3, 9, ':');
	OLED_ShowString(3, 14, "ppm");
	
	//显示“系统模式：”
	OLED_ShowChinese(4, 1, 24);
	OLED_ShowChinese(4, 2, 25);
	OLED_ShowChinese(4, 3, 26);
	OLED_ShowChinese(4, 4, 27);	
	OLED_ShowChar(4, 9, ':');
}

/**
  * @brief  显示菜单2的固定内容
  * @param  无
  * @retval 无
  */
void OLED_Menu2(void)
{
	//显示“火焰信号：”
	OLED_ShowChinese(1, 1, 8);
	OLED_ShowChinese(1, 2, 9);
	OLED_ShowChinese(1, 3, 10);
	OLED_ShowChinese(1, 4, 11);	
	OLED_ShowChar(1, 9, ':');
 
	
	
}

/**
  * @brief  显示菜单1的传感器数据
  * @param  无
  * @retval 无
  */
void SensorDataDisplay1(void)
{
	//显示温度数据
	OLED_ShowNum(1, 6, sensorData.temp, 2);
	//显示湿度数据
	OLED_ShowNum(1, 14, sensorData.humi, 2);
	
	//显示系统状态数据
	if (!mode)
	{
		OLED_ShowChinese(4, 6, 30);
		OLED_ShowChinese(4, 7, 31);		
	}
	else
	{
		OLED_ShowChinese(4, 6, 28);
		OLED_ShowChinese(4, 7, 29);			
	}
}

/**
  * @brief  显示菜单2的传感器数据
  * @param  无
  * @retval 无
  */
void SensorDataDisplay2(void)
{
	if (!sensorData.flame)
	{
		OLED_ShowChinese(1, 6, 36);
		OLED_ShowChinese(1, 7, 37);	
	}
	 
}

/**
  * @brief  显示阈值设置界面1的固定内容
  * @param  无
  * @retval 无
  */
void OLED_settingsPage1(void)
{
	 
	
	//显示“一氧阈值：”
	OLED_ShowChinese(4, 2, 42);
	OLED_ShowChinese(4, 3, 43);
	OLED_ShowChinese(4, 4, 18);
	OLED_ShowChinese(4, 5, 19);	
	OLED_ShowChar(4, 11, ':');
}

/**
  * @brief  显示阈值界面1的传感器数据
  * @param  无
  * @retval 无
  */
void settingsThresholdDisplay1(void)
{
	 
	
	//显示甲烷阈值数据
	OLED_ShowNum(4, 13, Sensorthreshold.methaneValue, 3);
}

/**
  * @brief  显示阈值界面的选择符号
  * @param  num 为显示的位置
  * @retval 无
  */
void OLED_Option(uint8_t num)
{
	switch(num)
	{
		case 1:	
			OLED_ShowChar(1,1,'>');
			OLED_ShowChar(2,1,' ');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		
		case 2:	
			OLED_ShowChar(1,1,' ');
			OLED_ShowChar(2,1,'>');
			OLED_ShowChar(3,1,' ');
			OLED_ShowChar(4,1,' ');
			break;
		 
		default: 
			break;
	}
}


/**
  * @brief  记录阈值界面下按KEY1的次数
  * @param  无
  * @retval 返回次数
  */
uint8_t SetSelection(void)
{
	static uint8_t count = 1;
	if(KeyNum == KEY_1)
	{
		KeyNum = 0;
		count++;
		if (count > 4)
		{
			count = 1;
		}
	}
	return count;
}

/**
  * @brief  对阈值界面的传感器阈值进行修改
  * @param  num 为当前用户需要更改的传感器阈值位置
  * @retval 无
  */
void ThresholdModification(uint8_t num)
{
	switch (num)
	{
		case 1:
			if (KeyNum == 3)
			{
				KeyNum = 0;
				Sensorthreshold.tempValue++;
				if (Sensorthreshold.tempValue > 99)
				{
					Sensorthreshold.tempValue = 0;
				}
			}
		 
			break;
			
		 
		
		default: break;		
	}
}

/**
  * @brief  系统自动模式下的运行
  * @param  无
  * @retval 无
  */
void AutomaticMode()
{
	if (mode)
	{
		/*当系统检测到有人时会开启LED灯，没人时自动关闭*/
		if (sensorData.people)
		{
			currentDataPoint.valueLED = 1;
			LED_ON();
			
		}
		 
		 
		
		/*系统检测到烟雾浓度高于阈值，会自动开启风扇并且关闭阀门*/
		if (sensorData.smog > Sensorthreshold.smogValue)
		{

			currentDataPoint.valuefan = 1;
			systemState.smogAlarm = 1;
			Relay_ON(1);			
			currentDataPoint.valuevalve = 1;
			Buzzer_ON();
			
		}
		else
		{
			systemState.smogAlarm = 0;
		}

	 

		/*控制舵机运行*/
		if ((sensorData.flame || sensorData.smog > Sensorthreshold.smogValue || sensorData.methane > Sensorthreshold.methaneValue)&& valveLockFlag == 0)
		{
			valveLockFlag = 1;
			Servo_SetAngle(90);

		}
		 
		/*控制风扇停止*/
		if (sensorData.temp < Sensorthreshold.tempValue && (sensorData.smog ) < Sensorthreshold.smogValue && (sensorData.methane ) < Sensorthreshold.methaneValue)
		{
			currentDataPoint.valuefan = 0;
			Relay_OFF(1);
		}
		 
	}
}

		
/**
  * @brief  传感器数据扫描
  * @param  无
  * @retval 无
  */
void SensorScan(void)
{
	DHT11_Read_Data(&sensorData.humi, &sensorData.temp);
	HC_SR501_Input(&sensorData.people);
	Get_Average_MQ2_PPM(&sensorData.smog); 
	
	if (Get_Average_Flame_Percent() < Sensorthreshold.flameValue)
	{
		 sensorData.flame = 0;
	}
	else
	{
		sensorData.flame = 1;
	}
}

int main(void)
{
	ADCX_Init();
	Timer2_Init(9,14398);
	Uart2_Init(9600);
	Uart1_Init(115200);
	 
	Key_Init();
	HC_SR501_Init();
	Servo_Init();
	
	Sensorthreshold.tempValue = FLASH_R(FLASH_START_ADDR);		//从指定页的地址读FLASH
	Sensorthreshold.flameValue = FLASH_R(FLASH_START_ADDR+2);	//从指定页的地址读FLASH
	Sensorthreshold.smogValue = FLASH_R(FLASH_START_ADDR+4);	//从指定页的地址读FLASH
 
	Servo_SetAngle(45);
	KeyNum = 0;
	gizwitsSetMode(WIFI_AIRLINK_MODE);
	Delay_ms(1000);
	
	while (1)
	{
		/*阈值上传至机智云云平台*/
		do
		{
			currentDataPoint.valuetempValue = Sensorthreshold.tempValue;
			currentDataPoint.valueflameValue = Sensorthreshold.flameValue;
			currentDataPoint.valuesmogValue = Sensorthreshold.smogValue;
			currentDataPoint.valuemethaneValue = Sensorthreshold.methaneValue;
			

			
		}while(0);
		
		IWDG_ReloadCounter(); 
			
		switch (menu)
		{
			case display_page1:
				
				SensorScan();	//获取传感器数据
				if (!displayPageNum)
				{
					SensorDataDisplay1();	//显示传感器1数据
					OLED_Menu1();	//显示主页面1固定信息					
				}
				else
				{
					SensorDataDisplay2();	//显示传感器2数据		
					OLED_Menu2();	//显示主页面2固定信息			
				}
				
				AutomaticMode();
				
			 
				

			case settingsPage:

				settingsThresholdDisplay1();	//显示传感器阈值1数据	
				OLED_settingsPage1();	//显示阈值设置界面1固定信息
				ThresholdModification(SetSelection());	//调节传感器阈值
				
				OLED_Option(SetSelection());	//获取按键次数，从而判断“>”显示位置
			
				if (KeyNum == KEY_2)	//判断用户是否按下退出按键
				{
					KeyNum = 0;
					OLED_Clear();	//清屏
					menu = display_page1;	//回到主页面1
					

					//存储修改的传感器阈值至flash内
					FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, Sensorthreshold.flameValue,
					Sensorthreshold.smogValue, Sensorthreshold.methaneValue);
					currentDataPoint.valuetempValue = Sensorthreshold.tempValue;
					currentDataPoint.valueflameValue = Sensorthreshold.flameValue;
					currentDataPoint.valuesmogValue = Sensorthreshold.smogValue;
					currentDataPoint.valuemethaneValue = Sensorthreshold.methaneValue;
				}
				break;
			default: break;
		}
		
		/*机智云阈值更改后保存至Flash*/
		if (valueFlashFlag)
		{
			valueFlashFlag = 0;
			FLASH_W(FLASH_START_ADDR, Sensorthreshold.tempValue, Sensorthreshold.flameValue,
							Sensorthreshold.smogValue, Sensorthreshold.methaneValue);			
		}
		 				
	}
}
