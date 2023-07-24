#include <Arduino.h>
#include <Wire.h>
#include <Ticker.h>

#include <Solution_algorithm.h>
#include <Madgwick.h>
#include <init.h>
#include <wifi_connection.h>
#include <bluetooth.h>
#include <led.h>

#define MADGWICK
#define MAX_BUF_CNT 128

Ticker timer;

char buffer[MAX_BUF_CNT];
int buf_len = 0;
float roll = 0, pitch = 0, yaw = 0;

static void Send_MSG();

void setup()
{
    /*打开串口和实现I2C通信*/
    Wire.begin(23, 5);//SDA->23,SCL->5

    Serial.begin(115200);
    
    MPU6050_setup_init();
    HMC5883L_setup_init();

#ifdef _WIFI_COMMUNICATION_
    wifi_setup();
#endif

#ifdef _BLUETOOTH_COMMUNICATION_
    bluetooth_setup();
#endif

    timer.attach(0.02f, Send_MSG);

#ifndef _MADGWICK_ALGORITHM_
    //计算偏移量
    offset_init();
#else
    Madgwick_init();
#endif

    delay(100);
}

void loop()
{
#ifndef _MADGWICK_ALGORITHM_
    get_Euler_angle(&roll, &pitch, &yaw);
#else
    get_Euler_angle_by_madgwick(&roll, &pitch, &yaw);
    //get_Euler_angle_by_madgwick_simple(&roll, &pitch, &yaw);
#endif

}


static void Send_MSG() 
{
#ifdef _BLUETOOTH_COMMUNICATION_
    blutooth_send_mag(roll, pitch, yaw);
#endif

#ifdef _WIFI_COMMUNICATION_
    wifi_communication_send_msg_loop(roll, pitch, yaw);
#endif
}