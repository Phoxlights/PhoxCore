#include <ESP8266WiFi.h>
#include "eventSender.h"
#include "network.h"
#include "event.h"
#include "eventRegistry.h"

// shared client for sending events
WiFiClient * client = NULL;

// TODO - minor version
int eventSend(IPAddress ip, int port, int version, event_opCode opCode, int length, void * body){
    if(!networkIsConnected()){
        Serial.println("cannot send event; no network");
        return 0;
    }

    if(!client){
        client = new WiFiClient();
    }

    if (client->connect(ip, port)) {
        Serial.printf("couldnt connect to %s:%i\n", ip.toString().c_str(), port);
        return 0;
    }

    // TODO - send event body
    EventHeader e = {
        .versionMajor = version,
        .versionMinor = 0,
        .responseId = 0,
        .requestId = 0,
        .opCode = opCode,
        .length = 0
    };
    const uint8_t* ptr = (byte*)(void*)&e;
    // TODO - error handling around client write
    client->write(ptr, sizeof(EventHeader));
    // TODO - leave client open?
    client->stop();

    return 1;
}
