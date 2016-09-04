#include <Arduino.h>
#include <Ticker.h>
#include <Adafruit_NeoPixel.h>
#include "lightstrip.h"

typedef struct LightStripState {
    Adafruit_NeoPixel * strip;
    int length;
    byte * buffer;
    float brightness;
    Ticker * ticker;
} LightStripState;

// writes buffer to strip, adjusts brightness
static void writePixels(byte * buffer, Adafruit_NeoPixel * strip, int numPx, float brightness){
    int i = 0;
    int r, g, b;
    float a;

    for(i = 0; i < numPx; i++){
        // wipe last value
		strip->setPixelColor(i, strip->Color(0,0,0));

        int offset = (i % numPx) * STRIDE;
        a = buffer[offset+3] / 255.0;
        r = (int)(buffer[offset] * a * brightness);
        g = (int)(buffer[offset+1] * a * brightness);
        b = (int)(buffer[offset+2] * a * brightness);
		strip->setPixelColor(i, strip->Color(r, g, b));
    }

    strip->show();
}

static void clearStrip(Adafruit_NeoPixel * strip, int numPx){
    for(int i = 0; i < numPx; i++){
		strip->setPixelColor(i, strip->Color(0,0,0));
    }
    strip->show();
}

void lightStripTick(LightStrip state){
    // TODO - dont update if not dirty
    writePixels(state->buffer, state->strip, state->length, state->brightness);
}

// creates a lightstrip guy
LightStrip lightStripCreate(int pin, int length, float brightness, byte * buffer){
    LightStrip state = (LightStrip)malloc(sizeof(LightStripState));

    state->strip = new Adafruit_NeoPixel(length, pin, NEO_GRB + NEO_KHZ800);
    state->strip->begin();
    clearStrip(state->strip, length);

    state->length = length;
    state->buffer = buffer;
    state->brightness = brightness;
    state->ticker = new Ticker();
    return state;
}

int lightStripFree(LightStrip state){
    state->ticker->detach();
    delete state->ticker;
    delete state->strip;
    free(state);
    return 1;
}

// update from buffer
// start updating lightstrip from buffer
int lightStripStart(LightStrip state){
    // TODO - get refresh rate from user?
    state->ticker->attach_ms(30, lightStripTick, state);
    return 1;
}

// stop updating lightstrip from buffer
int lightStripPause(LightStrip state){
    state->ticker->detach();
    return 1;
}

// clear / black out lightstrip
int lightStripClear(LightStrip state){
    clearStrip(state->strip, state->length);
    return 1;
}

int lightStripStop(LightStrip state){
    if(!lightStripPause(state)){
        return 0;
    }
    if(!lightStripClear(state)){
        return 0;
    }
    return 1;
}

