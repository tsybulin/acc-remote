#ifndef MQTT_H
#define MQTT_H

void mqtt_setup() ;
bool mqtt_connected() ;
void mqtt_reconnect() ;
void mqtt_loop() ;
void mqtt_toggle_entity(int id) ;

#endif
