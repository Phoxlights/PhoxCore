#ifndef LIGHTSTRIP_H
#define LIGHTSTRIP_H

// rgb is stride 3, rgba is stride 4
#define STRIDE 4

typedef struct LightStripState* LightStrip;

// creates a lightstrip guy
// NOTE - buffer should be (length * STRIDE) elements long
LightStrip lightStripCreate(int pin, int length, float brightness, byte * buffer);

// clean up bra
int lightStripFree(LightStrip state);

// start updating lightstrip from buffer
int lightStripStart(LightStrip state);

// stop updating lightstrip from buffer
int lightStripPause(LightStrip state);

// clear / black out lightstrip
int lightStripClear(LightStrip state);

// stop updating from buffer and clear lightstrip
int lightStripStop(LightStrip state);

// use lightStripStart to automatically tick light strip
// at a specified interval, but if you need fine
// control you can call this directly instead
void lightStripTick(LightStrip state);

// get length?

#endif
