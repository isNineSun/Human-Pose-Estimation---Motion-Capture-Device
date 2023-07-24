#ifndef _MADGWICK_
#define _MADGWICK_

#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_Sensor.h>

extern void Madgwick_init();

extern void get_Euler_angle_by_madgwick(float* roll, float* pitch, float* yaw);

extern void get_Euler_angle_by_madgwick_simple(float* roll, float* pitch, float* yaw);
#endif //_MADGWICK_