#include <stdlib.h>
#include <Adafruit_NeoPixel.h>
#include <Ticker.h>
#include "statuslight.h"

typedef struct StatusLightState {
    Adafruit_NeoPixel * strip;
    int length;
    byte color[3];
    int pattern[MAX_PATTERN_LENGTH];
    int patternIndex;
    Ticker * ticker;
} StatusLightState;

static void writeColorToStrip(StatusLight state, byte color[3]){
    for(int i = 0; i < state->length; i++){
        state->strip->setPixelColor(i, state->strip->Color(
            color[0],
            color[1],
            color[2]
        ));
    }
    state->strip->show();
}

static void updateStatusLight(StatusLight state){
    state->ticker->detach();

    // by default this is off
    byte color[3] = {0};

    // if even number, this is an on period
    if(!state->patternIndex % 2){
        memcpy(color, state->color, 3);
    }

    writeColorToStrip(state, color);

    // if last pattern, or if sentinel value, wrap
    if(state->patternIndex >= MAX_PATTERN_LENGTH || 
        state->pattern[state->patternIndex+1] == 0){
        state->patternIndex = 0;
    } else {
        state->patternIndex += 1;
    }

    // setup next tick
    state->ticker->attach_ms(state->pattern[state->patternIndex], updateStatusLight, state);
}

StatusLight statusLightCreate(int pin, int length){
    StatusLight state = (StatusLight)malloc(sizeof(StatusLightState));
    state->length = length;
    state->strip = new Adafruit_NeoPixel(length, pin, NEO_GRB + NEO_KHZ800);
    state->patternIndex = 0;
    state->ticker = new Ticker();
    state->strip->begin();
    byte black[3] = {0};
    writeColorToStrip(state, black);
    return state;
}

int statusLightSetPattern(StatusLight state, byte rgb[3], int pattern[MAX_PATTERN_LENGTH]){
    memcpy(state->color, rgb, 3);
    memcpy(state->pattern, pattern, sizeof(int) * MAX_PATTERN_LENGTH);
    state->patternIndex = 0;
    updateStatusLight(state);
    return 1;
}

int statusLightStop(StatusLight state){
    state->ticker->detach();
    // blank out status light
    byte nothin[] = {0,0,0};
    writeColorToStrip(state, nothin);
}
