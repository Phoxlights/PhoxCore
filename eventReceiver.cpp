#include "network.h"
#include "loop.h"
#include "event.h"
#include "eventReceiver.h"

#define MAX_EVENTS 20
#define MESSAGE_TIMEOUT 100
#define MAX_BODY_LENGTH 32

// EVENT REGISTRATION AND EXECUTION
struct eventMapNode {
    event_opCode opCode;
    eventCallback * handler;
};

typedef struct EventReceiverState {
    int versionMajor;
    int port;
    WiFiServer * server;
    eventMapNode ** eventMap;
    byte eventMapLength;
} EventReceiverState;

EventReceiverState * state = NULL;

static int eventFree(Event * e){
    Serial.print("freeing event\n");
    free(e->body);
    free(e->header);
    free(e);
}

// receive binary message from network
static int receiveMessage(WiFiClient client, byte * message, byte * body){
    Serial.println("receiving message");

    int i = 0;
    int timer = 0;
    // TODO - optionally receive only
    // the first part of event and trigger
    // the event when the body is not complete
    // so that the event body can be streamed
    while(i < sizeof(EventHeader)){
        if(!client.available()){
            if(timer > MESSAGE_TIMEOUT){
                Serial.printf("\nmessage receive timeout. received %i of %i bytes\n", i, sizeof(EventHeader));
                return 0;
            }
            timer++;
            Serial.print(".");
            delay(50);
            continue;
        }
        byte b = client.read();
        Serial.print(b);
        Serial.print(",");
        message[i] = b;
        i++;
    }

    // receive event body
    EventHeader * e;
    e = (EventHeader*)(void*)message;
    uint16_t bodyLength = e->length;
    if(bodyLength && bodyLength <= MAX_BODY_LENGTH){
        i = 0;
        timer = 0;
        Serial.printf("found body of length %i\n", bodyLength);
        while(i < bodyLength){
            if(!client.available()){
                if(timer > MESSAGE_TIMEOUT){
                    Serial.printf("\nmessage body receive timeout. received %i of %i bytes\n", i, bodyLength);
                    free(body);
                    return 0;
                }
                timer++;
                Serial.print(".");
                delay(50);
                continue;
            }
            byte b = client.read();
            Serial.print(b);
            body[i] = b;
            i++;
        }
    }

    client.flush();
    Serial.println("\nmessage complete");
    return 1;
}

// create an event struct from a binary
// message array
static Event * createEvent(byte * message, byte * body){
    EventHeader * header;
    header = (EventHeader*)(void*)message;
    Event * e = (Event*)malloc(sizeof(Event));
    e->header = header;
    e->body = (uint8_t*)body;
    return e;
}

static Event * eventReceive(WiFiClient client){
    byte * message = (byte*)malloc(sizeof(EventHeader));
    byte * body = (byte*)malloc(MAX_BODY_LENGTH);
    if(!receiveMessage(client, message, body)){
        Serial.println("failed to receive event");
        free(message);
        return NULL;
    }
    Event * e = createEvent(message, body);
    return e;
}

static int eventTrigger(Event * e){
    EventHeader * header = e->header;
    Serial.printf("Received event: %i\n", header->opCode);

    if(header->versionMajor != state->versionMajor){
        Serial.printf("Incompatible API ver: %i.%i\n", header->versionMajor, header->versionMajor);
        return 0;
    }

    for(byte i = 0; i < state->eventMapLength; i++){
        if(state->eventMap[i]->opCode == header->opCode){
            Serial.printf("fount event %i\n", header->opCode);
            state->eventMap[i]->handler(e);
            return 1;
        } 
    }

    Serial.printf("Couldnt find: %i\n", header->opCode);
    return 0;
}

static void eventReceiverTick(void * s){
    // listen for incoming event messages
    WiFiClient client = state->server->available();
    if(!client){
        return;
    }

    // TODO - max wait time
    // TODO - yield?
    while(!client.available()){
        delay(1);
    }

    Event * e = eventReceive(client);
    if(e == NULL){
        Serial.println("Failed to receive event; skipping");
        return;
    }

    if(!eventTrigger(e)){
        Serial.println("Couldn't trigger event");
        // NOTE - dont early return, need to call free
    }
    eventFree(e);
}


int eventListen(int versionMajor, int port){
    if(state != NULL){
        Serial.println("Cannot start event receive; already started");
        // TODO - is returning 1 here ok?
        return 1;
    }
    if(!networkIsConnected()){
        Serial.println("Cannot start event receiver; network not connected");
        return 0;
    }

    state = (EventReceiverState*)malloc(sizeof(EventReceiverState));
    state->versionMajor = versionMajor;
    state->port = port;

    // setup event map for registering events
    state->eventMap = (eventMapNode**)calloc(MAX_EVENTS, sizeof(eventMapNode*));
    state->eventMapLength = 0;

    state->server = new WiFiServer(port);
    state->server->begin();
    Serial.printf("began tcp server at %s:%i\n", networkGetIP().toString().c_str(), state->port);

    loopAttach(eventReceiverTick, 0, NULL);
    return 1;
}

int eventRegister(event_opCode opCode, eventCallback * handler){
    if(state == NULL){
        Serial.println("cannot register event; event receiver must be started first");
        return 0;
    }
    if(state->eventMapLength == MAX_EVENTS){
        Serial.printf("cannot register event; max events %i already reached\n", MAX_EVENTS);
        return 0;
    }

    Serial.printf("registering %i\n", opCode);

    struct eventMapNode * node = (struct eventMapNode*)malloc(sizeof(struct eventMapNode));
    node->opCode = opCode;
    node->handler = handler;

    state->eventMap[state->eventMapLength++] = node;
    return 1;
}
