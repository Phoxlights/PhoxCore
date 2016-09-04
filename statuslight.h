#ifndef STATUSLIGHT_H
#define STATUSLIGHT_H

#define MAX_PATTERN_LENGTH 10

typedef struct StatusLightState* StatusLight;

StatusLight statusLightCreate(int pin, int length);
int statusLightSetPattern(StatusLight state, byte rgb[3], int pattern[MAX_PATTERN_LENGTH]);
int statusLightStop(StatusLight state);

#endif
