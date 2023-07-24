#ifndef _WIFI_CONNECTION_
#define _WIFI_CONNECTION_

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
extern void wifi_communication_send_msg_loop(float roll, float pitch, float yaw);

extern void wifi_setup();

#endif //_WIFI_CONNECTION_