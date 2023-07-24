#include <Arduino.h>
#include <led.h>

#define Net_LED 2

void led_init()
{
    pinMode(Net_LED, OUTPUT);
}

void led_ctrl_connected()
{
    digitalWrite(Net_LED, HIGH);
}