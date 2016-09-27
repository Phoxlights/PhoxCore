#ifndef EVENTSENDER_H
#define EVENTSENDER_H

#include <ESP8266WiFi.h>
#include "event.h"
#include "eventRegistry.h"

// TODO - minor version
int eventSend(IPAddress ip, int port, int version, int opCode, int length, void * body, int responseId);
int eventSendC(WiFiClient * client, int version, int opCode, int length, void * body, int responseId);

#endif
