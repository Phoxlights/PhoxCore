#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <ESP8266WiFi.h>
#include "event.h"
#include "eventRegistry.h"

// TODO - minor version
// NOTE - there is a single global event listener instance
int eventListen(int version, int port);
int eventRegister(event_opCode opCode, eventCallback * handler);

#endif
