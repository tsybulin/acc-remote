#ifndef ENTITY_H
#define ENTITY_H

#include <Arduino.h>

struct Entity ;

typedef void (*entity_parser_t)(Entity*, byte*, unsigned int) ;
typedef void (*entity_changed_t)(Entity*) ;
typedef Entity* (*entity_get_t)(uint8) ;


struct Entity {
    const uint8 id;
    const char *stateTopic ;
    const char *stateValueTemplate ;
    const char *commandTopic ;
    bool onff = false ;
    entity_parser_t parser ;

    bool dimmable ;
    int brightness ;
    const char *brightnessCommandTopic ;
    const char *brightnessValueTemplate ;

    double value ;
    char state[24] ;

    public:
        Entity(const uint8 id, const char *stateTopic, const char *stateValueTemplate, const char *commandTopic, entity_parser_t parser) ;
        Entity(const uint8 id, const char *stateTopic, const char *commandTopic = NULL, entity_parser_t parser = NULL) ;
        Entity(const uint8 id,
               const char *stateTopic, const char *stateValueTemplate,
               const char *commandTopic, 
               const char *brightnessCommandTopic, const char *brightnessValueTemplate,
               entity_parser_t parser) ;
        Entity(const uint8 id, const char *stateTopic, entity_parser_t parser, const char *valueTemplate = NULL) ;
} ;

#endif
