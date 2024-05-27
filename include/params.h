#ifndef PARAMS_H
#define PARAMS_H

#define PIN_BTN_OUT D1
#define PIN_BTN_0   D2
#define PIN_BTN_1   D3
#define PIN_BTN_2   D4

#define INACTIVE_LED_DELAY  50
#define WIFI_LED_DELAY      250
#define MQTT_LED_DELAY      500

#define BUTTONS_STROBE_DELAY 100
#define BUTTONS_READ_DELAY   50

#define WIFI_SSID "panda"
#define WIFI_PASSWD "pandapandapan"
#define HOSTNAME "accremote"

#define MQTT_SERVER "172.16.101.251"
#define MQTT_CLIENT HOSTNAME "-123"
#define MQTT_USER "tasmota"
#define MQTT_PASSWD "123456"

#endif
