#include <init.h>

Adafruit_MPU6050 mpu;
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void MPU6050_setup_init()
{
    //MPU6050配置
    while (!mpu.begin())
    {
      Serial.println("Failed to find MPU6050 chip");
    }
    Serial.println("MPU6050 Found!");
#ifndef _MADGWICK_ALGORITHM_
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);//加速度量程±2G
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);//角速度量程±250°/s
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);//采样频率21Hz
    mpu.setI2CBypass(true);
#else
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);//加速度量程±2G
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);//角速度量程±250°/s
    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);//采样频率21Hz
    mpu.setI2CBypass(true);
#endif
}

void HMC5883L_setup_init()
{
    if(!mag.begin())
    {
        /* There was a problem detecting the HMC5883 ... check your connections */
        Serial.println("Failed to find MPU6050_1 chip");
        while(1);
    }
}
