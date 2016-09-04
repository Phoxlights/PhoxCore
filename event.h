#ifndef EVENT_H
#define EVENT_H

// TODO - info about sender and recipient?
typedef struct EventHeader{
    uint16_t opCode;
    uint16_t version;
    uint16_t length;
} EventHeader;

typedef struct Event {
    EventHeader * header;
    uint8_t * body;
} Event;

#endif
