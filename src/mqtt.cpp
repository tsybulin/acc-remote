#include "mqtt.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "params.h"
#include "entity.h"

WiFiClient wiFiClient ;
PubSubClient mqttClient(wiFiClient) ;

extern entity_changed_t entity_changed ;

void switch_parser(Entity *e, byte* payload, unsigned int length) {
    if (strncmp((char *)payload, "ON", 2) == 0) {
        e->onff = true ;
    } else {
        e->onff = false ;
    }
}

void light_json_parser(Entity *e, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc ;
    char json[length+1] ;
    strncpy(json, (char *)payload, length) ;
    json[length] = '\0' ;
    DeserializationError error = deserializeJson(doc, json) ;
    if (error) {
        return ;
    }

    const char *onoff = doc[e->stateValueTemplate] ;
    if (onoff) {
        if (strncmp(onoff, "ON", 2) == 0) {
            e->onff = true ;
        } else {
            e->onff = false ;
        }
    }

    JsonVariant brightness = doc[e->brightnessValueTemplate] ;
    if (!brightness.isNull()) {
        e->brightness = brightness.as<int>() ;
    }
}

Entity entities[] = {
    {1, "cabinet/stat/backlight/RESULT", "POWER", "cabinet/cmnd/backlight/POWER", "cabinet/cmnd/backlight/DIMMER", "Dimmer", light_json_parser},
    {2, "cabinet/stat/outlet1/POWER", "cabinet/cmnd/outlet1/POWER", switch_parser},
    {3, "hall/stat/outlet2/POWER", "hall/cmnd/outlet2/POWER", switch_parser}
} ;

bool mqtt_connected() {
    return mqttClient.connected() ;
}


void mqttHandler(char* topic, byte* payload, unsigned int length) {
for (int i = 0; i < (int)(sizeof(entities) / sizeof(Entity)); i++) {
        Entity *e = &entities[i] ;

        if (strcmp(topic, e->stateTopic) != 0) {
            continue ;
        }
        
        if (e->parser == NULL) {
            continue ;
        }
        
        e->parser(e, payload, length) ;
        if (entity_changed != NULL) {
            entity_changed(e) ;
        }
    }
}

void mqtt_setup() {
    mqttClient.setServer(MQTT_SERVER, 1883) ;
    mqttClient.setCallback(mqttHandler) ;
}


bool connecting = false ;

void mqtt_reconnect() {
    if (connecting) {
        return ;
    }

    if (mqttClient.connected()) {
        return ;
    }

    connecting = true ;

    if (mqttClient.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWD)) {
        for (Entity e : entities) {
            mqttClient.subscribe(e.stateTopic) ;
        }

        for (Entity e : entities) {
            if (e.commandTopic == NULL) {
                continue ;
            }

            mqttClient.publish(e.commandTopic, "", false) ;
        }
    }

    connecting = false ;
}

void mqtt_loop() {
    mqttClient.loop() ;
}

void mqtt_toggle_entity(int id) {
    Entity *e = &entities[id] ;
    e->onff = !e->onff ;

    if (e->commandTopic) {
        mqttClient.publish(e->commandTopic, e->onff ? "ON" : "OFF", false) ;
    }
}
