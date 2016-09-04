#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include "ESP8266WiFi.h"
#include "event.h"
#include "eventRegistry.h"

typedef void(eventCallback)(Event * e);
int eventReceiverStart(int version, int port);
int eventReceiverRegister(event_opCode opCode, eventCallback * handler);

#endif
