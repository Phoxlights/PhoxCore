#include <string.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>
#include "network.h"

enum networkStatus {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    UNEXPECTED_DISCONNECT,
    ERROR,
};

IPAddress NO_IP = IPAddress(0U);

static struct networkState {
    char ssid[SSID_MAX];
    char pass[PASS_MAX];
    IPAddress apIP;
    bool usingDNS;
    bool isAP;
    bool isSTA;
    networkStatus status;
    IPAddress ip;
    Ticker ticker;
} state = {0};

// workaround to force wifi off per
// https://github.com/esp8266/Arduino/issues/644
static void wifiOff(){
    WiFi.mode(WIFI_OFF);
    //WiFi.forceSleepBegin();
    delay(1);
}
// workaround to force wifi back on per
// https://github.com/esp8266/Arduino/issues/644
static void wifiOn(){
    //WiFi.forceSleepWake();
    WiFi.mode(WIFI_STA);  
    //WiFi.forceForceWake();
}

static void resetState(){
    strcpy(state.ssid, "");
    strcpy(state.pass, "");
    state.usingDNS = false;
    state.apIP = NO_IP;
    state.isAP = false;
    state.isSTA = false;
    state.status = DISCONNECTED;
    state.ip = NO_IP;
}

static void tick(){
    // TODO - check network status, reconnect, update state, etc
}

static int stopAP(){
    if(state.status == CONNECTED){
        if(!WiFi.softAPdisconnect(true)){
            resetState();
            state.status = ERROR;
            Serial.println("couldnt stop AP. resetting things real hard");
            return 0;
        }
    // TODO - handle other statuses
    // TODO - if connecting, stop connection attempts
    } else {
        Serial.println("cant stop ap because it is not connected");
        return 0;
    }

    resetState();
    state.status = DISCONNECTED;
    return 1;
}

static int stopSTA(){
    if(state.status == CONNECTED){
        if(!WiFi.disconnect(true)){
            resetState();
            state.status = ERROR;
            Serial.println("couldnt disconnect. resetting things real hard");
            return 0;
        }
    // TODO - handle other statuses
    // TODO - if connecting, stop connection attempts
    } else {
        Serial.println("cant disconnect because we aint connected");
        return 0;
    }

    resetState();
    state.status = DISCONNECTED;
    return 1;
}

int networkConnect(char * ssid, char * pass){
    if(state.status != DISCONNECTED){
        Serial.println("couldn't create network; network is not in disconnected state");
        return 0;
    }

    resetState();
    // HACK - works around https://github.com/tzapu/WiFiManager/issues/31
    // which is supposed to be fixed, but im still seeing it :/
    WiFi.mode(WIFI_OFF);  
    delay(200);

    wifiOn();
    Serial.printf("Connecting to %s\n", ssid);
    WiFi.begin(ssid, pass);
    state.status = CONNECTING;
    
    int status = WiFi.status();
    int count = 0;
    // TODO - move connection into main loop
    while(true){
        status = WiFi.status();

        // if we're connected, yay!
        if(status == WL_CONNECTED){
            // hold on to config for reconnect
            strcpy(state.ssid, ssid);
            strcpy(state.pass, pass);

            state.isSTA = true;
            state.ip = WiFi.localIP();
            Serial.printf("\nConnected to %s, got IP %s\n", ssid, state.ip.toString().c_str());
            state.status = CONNECTED;
            return 1;
        }

        // if we've run out of tries, game over man
        if(count >= MAX_CONNECTION_ATTEMPTS){
            Serial.printf("\nGave up connecting to %s after %i attempts\n", ssid, MAX_CONNECTION_ATTEMPTS);
            wifiOff();
            resetState();
            state.status = DISCONNECTED;
            return 0;
        }

        // wait a tick and try again
        count++;
        Serial.print(status);
        delay(500);
    }

    // wow, something went REAL bad.
    resetState();
    state.status = DISCONNECTED;
    Serial.printf("Shouldn't be here...\n");
    return 0;
}

int networkCreate(char * ssid, char * pass, IPAddress apIP){
    if(state.status != DISCONNECTED){
        Serial.println("couldn't create network; network is not in disconnected state");
        return 0;
    }

    resetState();

    // HACK - works around https://github.com/tzapu/WiFiManager/issues/31
    // which is supposed to be fixed, but im still seeing it :/
    WiFi.mode(WIFI_OFF);  
    delay(200);

    wifiOn();
    WiFi.mode(WIFI_AP);
    // TODO - get netmask from user
    if(!WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 0, 0))){
        Serial.println("couldn't configure AP");
        return 0;
    }
    if(!WiFi.softAP(ssid, pass)){
        Serial.println("couldn't start AP");
        return 0;
    }

    // hold on to config for reconnect
    strcpy(state.ssid, ssid);
    strcpy(state.pass, pass);
    state.apIP = apIP;

    state.isAP = true;
    state.ip = WiFi.softAPIP();
    Serial.printf("AP ssid: %s, ip: %s\n", ssid, state.ip.toString().c_str());
    state.status = CONNECTED;
    return 1;
}

int networkStop(){
    // TODO - stop network, disconnect AP, etc
    if(state.isAP){
        if(!stopAP()){
            Serial.println("couldn't stop network");
            return 0;
        }
    } else if(state.isSTA){
        if(!stopSTA()){
            Serial.println("couldn't stop network");
            return 0;
        }
    } else {
        Serial.println("couldnt stop network because i dont even");
        return 0;
    }
    return 1;
}

int networkOff(){
    // TODO - disconnect
    wifiOff();
}

IPAddress networkGetIP(){
    if(state.status == CONNECTED){
        return state.ip;
    }
    return NO_IP;
}

int networkAdvertise(char * hostname){
    if(state.status != CONNECTED){
        Serial.println("cannot advertise ip; not connected");
        return 0;
    }

    Serial.println("beginning mdns");
    // TODO - max attempts
    while(!MDNS.begin(hostname)){
        Serial.print(".");
        delay(1000);
    }
    Serial.printf("\nhostname %s bound to ip %s\n", hostname, state.ip.toString().c_str());

    // TODO - add services?
    //MDNS.enableArduino(_port);

    state.usingDNS = true;

    return 1;
}

bool networkIsConnected(){
    return state.status == CONNECTED;
}
