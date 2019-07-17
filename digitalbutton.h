#ifndef DIGITALBUTTON_H
#define DIGITALBUTTON_H

typedef struct DigitalButtonState* DigitalButton;
typedef void(buttonCallback)();

DigitalButton buttonCreate(int pin);

void digitalButtonTick(DigitalButton state);

void buttonOnDown(DigitalButton state, buttonCallback cb, int debounceTime);
void buttonOnUp(DigitalButton state, buttonCallback cb, int debounceTime);
// handler for quick button down then up
void buttonOnTap(DigitalButton state, buttonCallback cb, int debounceTime);
// handler for button down for extended period of time
// NOTE - holdMin is in ms
void buttonOnHold(DigitalButton state, buttonCallback cb, int holdMin);
//button state, callback function, number of shakes, shake window in milliseconds
void buttonOnShake(DigitalButton state, buttonCallback cb, int shakeCount, int shakeWin);

#endif
