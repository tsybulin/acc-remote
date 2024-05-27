#include <Arduino.h>
#include <Ticker.h>

#include "params.h"
#include "wifi.h"
#include "mqtt.h"
#include "entity.h"

Ticker blinker ;
Ticker buttonsstrobe ;
Ticker buttonsreader ;
Ticker commander ;

volatile enum State {
    INACTIVE = 0,
    WIFI = 1,
    MQTT = 2
} state ;

int buttons = 0x666 ;

void entityChanged(Entity *entity) {
    // Serial.printf("entity changed id:%d onoff:%d\n", entity->id, entity->onff) ;
}

entity_changed_t entity_changed = entityChanged ;

void ticker_blinker() {
    digitalWrite(LED_BUILTIN_AUX, !digitalRead(LED_BUILTIN_AUX)) ;
}

void ticker_commander(int id) {
    mqtt_toggle_entity(id) ;
}

void ticker_buttonsreader() {
    int btns = (!digitalRead(PIN_BTN_0)) | (!digitalRead(PIN_BTN_1) << 1) | (!digitalRead(PIN_BTN_2) << 2) ;
    if (buttons != btns) {
        buttons = btns ;
        // Serial.printf("buttons: 0x%04X\n", buttons) ;

        if (buttons == 0x01) {
            commander.once_ms(100, ticker_commander, 0) ;
        } else if (buttons == 0x02) {
            commander.once_ms(100, ticker_commander, 1) ;
        } else if (buttons == 0x04) {
            commander.once_ms(100, ticker_commander, 2) ;
        }
    }

    digitalWrite(PIN_BTN_OUT, HIGH) ;
}

void ticker_buttonsstrobe() {
    if (state != MQTT) {
        return ;
    }

    digitalWrite(PIN_BTN_OUT, LOW) ;
    buttonsreader.once_ms(BUTTONS_READ_DELAY, ticker_buttonsreader) ;
}

void setup() {
    // Serial.begin(57600) ;
    // Serial.println("AccRemote") ;

    pinMode(LED_BUILTIN_AUX, OUTPUT) ;
    digitalWrite(LED_BUILTIN_AUX, HIGH) ;

    pinMode(PIN_BTN_OUT, OUTPUT) ;
    digitalWrite(PIN_BTN_OUT, HIGH) ;

    pinMode(PIN_BTN_0, INPUT_PULLUP) ;
    pinMode(PIN_BTN_1, INPUT_PULLUP) ;
    pinMode(PIN_BTN_2, INPUT_PULLUP) ;


    state = INACTIVE ;
    blinker.attach_ms(INACTIVE_LED_DELAY, ticker_blinker) ;

    buttonsstrobe.attach_ms(BUTTONS_STROBE_DELAY, ticker_buttonsstrobe) ;

    wifi_connect() ;
    mqtt_setup() ;
}

void loop() {
    enum State newState = INACTIVE ;
    if (wifi_connected()) {
        newState = WIFI ;

        if (mqtt_connected()) {
            newState = MQTT ;
        } else {
            mqtt_reconnect() ;
        }
    }

    if (state != newState) {
        state = newState ;
        blinker.detach() ;

        switch (state) {
            case INACTIVE:
                blinker.attach_ms(INACTIVE_LED_DELAY, ticker_blinker) ;
                break ;

            case WIFI:
                blinker.attach_ms(WIFI_LED_DELAY, ticker_blinker) ;
                break ;

            case MQTT:
                blinker.attach_ms(MQTT_LED_DELAY, ticker_blinker) ;
                break ;
            
            default:
                blinker.attach_ms(INACTIVE_LED_DELAY, ticker_blinker) ;
                break ;
        }
    }

    mqtt_loop() ;

    ESP.wdtFeed() ;
}
