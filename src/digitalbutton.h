#ifndef DIGITALBUTTON_H
#define DIGITALBUTTON_H

typedef struct DigitalButtonState* DigitalButton;
typedef void(buttonCallback)();

DigitalButton buttonCreate(int pin, int debounceTime);

void buttonOnDown(DigitalButton state, buttonCallback cb);
void buttonOnUp(DigitalButton state, buttonCallback cb);
// handler for quick button down then up
void buttonOnTap(DigitalButton state, buttonCallback cb);
// handler for button down for extended period of time
// NOTE - holdMin is in ms
void buttonOnHold(DigitalButton state, buttonCallback cb, int holdMin);

#endif
