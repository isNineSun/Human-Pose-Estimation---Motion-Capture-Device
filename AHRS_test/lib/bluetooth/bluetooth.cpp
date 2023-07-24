#include <bluetooth.h>

static BluetoothSerial SerialBT;

void bluetooth_setup()
{
    //蓝牙配置
    SerialBT.begin("Ninesun"); 
    Serial.printf("BT initial ok and ready to pair. \r\n");
    delay(100);
}

void blutooth_send_mag(float roll, float pitch, float yaw)
{
    char buffer[128];
    int buf_len = 0;
    const uint8_t* buf = NULL;

    buf_len = snprintf(buffer, 128, "%6.2f,%6.2f,%7.2f", roll, pitch, yaw);

    if(strlen(buffer) > 0)
    {
        buf = (uint8_t*)buffer;
    }
    SerialBT.write(buf, buf_len);

    // Serial.print("roll: ");
    // Serial.print(roll);
    // Serial.print(",");
    // Serial.print("pitch: ");
    // Serial.print(pitch);
    // Serial.print(",");
    // Serial.print("yaw: ");
    // Serial.println(yaw);
}