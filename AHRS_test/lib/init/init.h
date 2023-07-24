#ifndef _INIT_H_
#define _INIT_H_

#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_Sensor.h>

#define _MADGWICK_ALGORITHM_

//#define _BLUETOOTH_COMMUNICATION_
#define _WIFI_COMMUNICATION_

extern Adafruit_MPU6050 mpu;
extern Adafruit_HMC5883_Unified mag;

extern void Timer_init(void timer_cb());

extern void MPU6050_setup_init();

extern void HMC5883L_setup_init();

#endif //_INIT_H_