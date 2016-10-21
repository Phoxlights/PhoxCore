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

static const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

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

        // apply gamma curve
        r = gamma8[r];
        g = gamma8[g];
        b = gamma8[b];

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

