#ifndef EVENT_H
#define EVENT_H

#include <WiFiClient.h>

typedef struct EventHeader{
    uint16_t versionMajor;
    uint16_t versionMinor;
    uint16_t responseId;
    uint16_t requestId;
    uint16_t opCode;
    uint16_t length;
} EventHeader;

typedef struct Event {
    EventHeader * header;
    uint8_t * body;
} Event;

typedef struct Request {
    IPAddress remoteIP;
    uint16_t  remotePort;
    // TODO - use an interface around this class
    WiFiClient * client;
} Request;


typedef void(eventCallback)(Event * e, Request * r);

#endif
