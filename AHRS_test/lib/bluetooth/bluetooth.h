#ifndef _BLUETOOTH_
#define _BLUETOOTH_

#include <BluetoothSerial.h>

extern void bluetooth_setup();

extern void blutooth_send_mag(float roll, float pitch, float yaw);
#endif //_BLUETOOTH_