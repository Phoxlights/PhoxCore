#include <ESP8266WiFi.h>
#include "network.h"
#include "eventSender.h"
#include "event.h"

typedef struct EventSenderState {
    IPAddress ip;
    int port;
    int version;
    WiFiClient * client;
} EventSenderState;

EventSender eventSenderCreate(IPAddress ip, int port, int version){
    EventSenderState * state = (EventSenderState*)malloc(sizeof(EventSenderState));
    state->ip = ip;
    state->port = port;
    state->version = version;
    state->client = new WiFiClient();
    return state;
}

// TODO - string instead of opcode
int eventSend(EventSender state, int opCode){
    if(!networkIsConnected()){
        Serial.println("cannot send event; no network");
        return 0;
    }

    if (!state->client->connect(state->ip, state->port)) {
        Serial.printf("couldnt connect to %s:%i\n", state->ip.toString().c_str(), state->port);
        return 0;
    }

    // TODO - send event body
    EventHeader e = {opCode, state->version, 0};
    const uint8_t* ptr = (byte*)(void*)&e;
    // TODO - error handling around client write
    state->client->write(ptr, sizeof(EventHeader));
    // TODO - leave client open?
    state->client->stop();

    return 1;
}
