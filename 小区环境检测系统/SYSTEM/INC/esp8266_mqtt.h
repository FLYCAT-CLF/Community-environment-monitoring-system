#ifndef __ES8266_MQTT_H
#define __ES8266_MQTT_H

#include "stm32f4xx.h"


#define WEATHER_URL  		"v.api.aa1.cn"
#define WEATHER_URL1  		"api.seniverse.com"
#define WEATHER_PORT  	80

void mqtt_cjson_parse(const char *json_string,char *s);
void mqtt_cjson_parse1(const char *json_string,char *s);
#endif
