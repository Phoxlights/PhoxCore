#include "ArduinoOTA.h"
#include "Ticker.h"
#include "loop.h"
#include "network.h"
#include "ota.h"

#define OTA_CHECK_FREQ 200

static struct OTAState {
    callback * onStart;
    callback * onEnd;
    progressCallback * onProgress;
    errorCallback * onError;
} state = {0};

void otaOnStart(callback * cb){
    state.onStart = cb;
}
void otaOnEnd(callback * cb){
    state.onEnd = cb;
}
void otaOnProgress(progressCallback * cb){
    state.onProgress = cb;
}
void otaOnError(errorCallback * cb){
    state.onError = cb;
}

static void onStart(){
    Serial.println("OTA start");
    if(state.onStart){
        (*state.onStart)();
    }
}
static void onEnd(){
    Serial.println("OTA end");
    if(state.onEnd){
        (*state.onEnd)();
    }
}
static void onProgress(unsigned int progress, unsigned int total){
    Serial.printf("OTA progress: %u%%\r", (progress / (total / 100)));
    if(state.onProgress){
        (*state.onProgress)(progress, total);
    }
}
static void onError(ota_error_t error){
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    if(state.onError){
        (*state.onError)(error);
    }
}

static void checkUpdate(void * state){
    ArduinoOTA.handle();
}
// TODO - stopCheckingUpdate
static void startCheckingUpdate(){
    loopAttach(&checkUpdate, 0, NULL);
}

int otaStart(){
    if(!networkIsConnected()){
        Serial.println("Not starting OTA: not connected");
        return 0;
    }

    ArduinoOTA.onStart(&onStart);
    ArduinoOTA.onEnd(&onEnd);
    ArduinoOTA.onProgress(&onProgress);
    ArduinoOTA.onError(&onError);

    if(!ArduinoOTA.begin(false)){
        Serial.println("Failed to start OTA");
        return 0;
    }

    startCheckingUpdate();
    return 1;
}
