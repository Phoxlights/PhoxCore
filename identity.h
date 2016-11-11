#ifndef IDENTITY_H
#define IDENTITY_H

#include <Esp.h>

typedef struct Identity {
    uint32_t model;
    uint32_t serial;
    uint16_t bin;
    uint16_t eventVer;
    uint16_t dbVer;
} Identity;

bool identityIsDefined(Identity * id);
int identityCopy(Identity * target, Identity * src);

#endif
