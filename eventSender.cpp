#include <ESP8266WiFi.h>
#include "eventSender.h"
#include "network.h"
#include "event.h"
#include "eventRegistry.h"

// shared client for sending events
WiFiClient * client = NULL;

static uint16_t id = 0;

static int getId(){
    return id++;
}

// The C is for Client!
// TODO - minor version
// TODO - wrap/abstract WiFiClient
int eventSendC(WiFiClient * client, int version, int opCode, int length, void * body, int responseId){
    // TODO - verify client is connected

    EventHeader e = {
        .versionMajor = version,
        .versionMinor = 0,
        .responseId = responseId,
        .requestId = getId(),
        .opCode = opCode,
        .length = length,
    };

    // write header
    const uint8_t* headerPtr = (byte*)(void*)&e;
    // TODO - error handling around client write
    client->write(headerPtr, sizeof(EventHeader));

    // write body
    const uint8_t* bodyPtr = (byte*)body;
    // TODO - error handling around client write
    client->write(bodyPtr, length);

    // TODO - leave client open?
    client->stop();

    return 1;
}

// TODO - minor version
int eventSend(IPAddress ip, int port, int version, int opCode, int length, void * body, int responseId){
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

    return eventSendC(client, version, opCode, length, body, responseId);
}
