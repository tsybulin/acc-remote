#include "wifi.h"
#include "params.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>

const char* wifi_ssid PROGMEM = WIFI_SSID ;
const char* wifi_passwd PROGMEM = WIFI_PASSWD ;

void wifi_connect() {
    if (WiFi.isConnected()) {
        return ;
    }

    WiFi.mode(WIFI_STA) ;
    WiFi.hostname(HOSTNAME) ;
    WiFi.begin(FPSTR(wifi_ssid), FPSTR(wifi_passwd)) ;
    // while (WiFi.status() != WL_CONNECTED) {
    //     delay(500) ;
    // }
    WiFi.setAutoReconnect(true) ;
}

bool wifi_connected() {
    return WiFi.isConnected() ;
}
