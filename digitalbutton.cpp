#include <Arduino.h>
#include <stdlib.h>
#include "loop.h"
#include "digitalbutton.h"

typedef struct DigitalButtonState {
    int pin;
    int holdMin;
    buttonCallback * onDown;
    buttonCallback * onUp;
    buttonCallback * onTap;
    buttonCallback * onHold;

    // if button is currently down, this was
    // when the down press began
    unsigned long currentDown;
    bool isHold;
} DigitalButtonState;

static void triggerDown(DigitalButtonState * state){
    if(state->onDown != NULL){
        state->onDown();
    }
}
static void triggerUp(DigitalButtonState * state){
    if(state->onUp != NULL){
        state->onUp();
    }
}
static void triggerTap(DigitalButtonState * state){
    if(state->onTap != NULL){
        state->onTap();
    }
}
static void triggerHold(DigitalButtonState * state){
    if(state->onHold != NULL){
        state->onHold();
    }
}

static void digitalButtonTick(void * s){
    DigitalButtonState * state = (DigitalButtonState*)s;
    int buttonPosition = digitalRead(state->pin);

    // button is currently pressed
    // TODO - this assumes were using pullup
    if(buttonPosition == LOW){

        // button has just been pressed
        if(!state->currentDown){
            triggerDown(state);
            state->currentDown = millis();

        // button has continued to be pressed
        } else {
            // if this has not become a hold event yet,
            // and the elapsed hold time meets the
            // hold min, make this a hold event
            if(!state->isHold && state->holdMin &&
                    millis() - state->currentDown >= state->holdMin){
                triggerHold(state);
                state->isHold = true;
            }
        }

    // button aint pressed right now guys.
    } else if(buttonPosition == HIGH){

        // button was just released
        if(state->currentDown){
            triggerUp(state);

            // if this wasnt a hold, it's a tap
            if(!state->isHold){
                triggerTap(state);
            }

            // clear event-specific state
            state->currentDown = 0;
            state->isHold = false;

        // button remains unprexed
        } else {
            // :(
        }
    }
}

DigitalButton buttonCreate(int pin, int debounceTime){
    DigitalButtonState * state = (DigitalButtonState*)calloc(1, sizeof(DigitalButtonState)); 
    state->pin = pin;

    // TODO - configurable pinMode?
    pinMode(pin, INPUT_PULLUP);
    digitalWrite(pin, HIGH);

    loopAttach(digitalButtonTick, debounceTime, state);
    return state;
}

void buttonOnDown(DigitalButton state, buttonCallback cb){
    state->onDown = cb;
}
void buttonOnUp(DigitalButton state, buttonCallback cb){
    state->onUp = cb;
}
void buttonOnTap(DigitalButton state, buttonCallback cb){
    state->onTap = cb;
}
void buttonOnHold(DigitalButton state, buttonCallback cb, int holdMin){
    state->onHold = cb;
    state->holdMin = holdMin;
}
