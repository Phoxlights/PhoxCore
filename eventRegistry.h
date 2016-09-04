#ifndef EVENTREGISTRY_H
#define EVENTREGISTRY_H

typedef enum {
    BRAKE_ON = 0,
    BRAKE_OFF = 1,
    SIGNAL_R_ON = 2,
    SIGNAL_R_OFF = 3,
    SIGNAL_L_ON = 4,
    SIGNAL_L_OFF = 5,
    PING = 1000,
    SET_TAILLIGHT_OFFSET = 2000,
    SET_BUTTON_PIN = 2100,
    SET_TAILLIGHT_PIN = 2200,
    SET_STATUS_PIN = 2300,
    SET_TOGGLE_R_PIN = 2400,
    SET_TOGGLE_L_PIN = 2500,
    SET_BRAKE_PIN = 2600,
    SET_NETWORK_MODE = 2700,
    SET_DEFAULT_CONFIG = 3000,
    PAUSE_TAILLIGHT = 4000,
    RESUME_TAILLIGHT = 4100,
    SET_PIXEL = 4200,
    NEXT_PRESET = 4300
} event_opCode;

// TODO - dynamically register events instead of
// hard coded enum
// TODO - event string names

#endif