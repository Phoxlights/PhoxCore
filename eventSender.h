#ifndef EVENTSENDER_H
#define EVENTSENDER_H

typedef struct EventSenderState* EventSender;

EventSender eventSenderCreate(IPAddress ip, int port, int version);
int eventSend(EventSender sender, int opCode);

#endif
