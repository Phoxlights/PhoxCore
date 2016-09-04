#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

typedef enum TogglePosition {
    NEUTRAL = 1,
    LEFT,
    RIGHT
};

typedef struct ToggleSwitchState* ToggleSwitch;
typedef void(toggleCallback)(TogglePosition lastPosition);

ToggleSwitch toggleCreate(int pinLeft, int pinRight, int debounceTime);

void toggleOnNeutral(ToggleSwitch state, toggleCallback cb);
void toggleOnLeft(ToggleSwitch state, toggleCallback cb);
void toggleOnRight(ToggleSwitch state, toggleCallback cb);

#endif
