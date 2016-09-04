#include <Arduino.h>
#include <stdlib.h>
#include "loop.h"
#include "toggleswitch.h"

typedef struct ToggleSwitchState {
    int pinLeft;
    int pinRight;
    toggleCallback * onNeutral;
    toggleCallback * onLeft;
    toggleCallback * onRight;
    TogglePosition lastPosition;
} ToggleSwitchState;

static void triggerNeutral(ToggleSwitchState * state){
    if(state->onNeutral != NULL){
        state->onNeutral(state->lastPosition);
    }
}
static void triggerLeft(ToggleSwitchState * state){
    if(state->onLeft != NULL){
        state->onLeft(state->lastPosition);
    }
}
static void triggerRight(ToggleSwitchState * state){
    if(state->onRight != NULL){
        state->onRight(state->lastPosition);
    }
}

static void toggleSwitchTick(void * s){
    ToggleSwitchState * state = (ToggleSwitchState*)s;
    int leftPosition = digitalRead(state->pinLeft);
    int rightPosition = digitalRead(state->pinRight);

    // NOTE - assumes pullup
    // if this was not previously at netural
    // a change has occured and should be triggered
    if(leftPosition == HIGH && rightPosition == HIGH &&
            state->lastPosition != NEUTRAL){
        triggerNeutral(state);
        state->lastPosition = NEUTRAL;

    // if this was not previously at left
    // a change has occured and should be triggered
    } else if(leftPosition == LOW && rightPosition == HIGH &&
            state->lastPosition != LEFT){ 
        triggerLeft(state);
        state->lastPosition = LEFT;

    // if this was not previously at right
    // a change has occured and should be triggered
    } else if(leftPosition == HIGH && rightPosition == LOW &&
            state->lastPosition != RIGHT){
        triggerRight(state);
        state->lastPosition = RIGHT;

    } else {
        // :(
    }
}

ToggleSwitch toggleCreate(int pinLeft, int pinRight, int debounceTime){
    ToggleSwitchState * state = (ToggleSwitchState*)calloc(1, sizeof(ToggleSwitchState)); 
    state->pinLeft = pinLeft;
    state->pinRight = pinRight;

    // set an invalid last position to force
    // it to always update the first time
    // NOTE - this means a callback's lastPosition
    // argument may effectively be NULL
    state->lastPosition = (TogglePosition)0;

    // TODO - configurable pinMode?
    pinMode(pinLeft, INPUT_PULLUP);
    digitalWrite(pinLeft, HIGH);
    pinMode(pinRight, INPUT_PULLUP);
    digitalWrite(pinRight, HIGH);

    loopAttach(toggleSwitchTick, debounceTime, state);
    return state;
}

void toggleOnNeutral(ToggleSwitch state, toggleCallback cb){
    state->onNeutral = cb;
}
void toggleOnLeft(ToggleSwitch state, toggleCallback cb){
    state->onLeft = cb;
}
void toggleOnRight(ToggleSwitch state, toggleCallback cb){
    state->onRight = cb;
}
