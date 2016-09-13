#ifndef EVENTSENDER_H
#define EVENTSENDER_H

#include <ESP8266WiFi.h>
#include "event.h"
#include "eventRegistry.h"

// TODO - minor version
int eventSend(IPAddress ip, int port, int version, event_opCode opCode, int length, void * body);

#endif
