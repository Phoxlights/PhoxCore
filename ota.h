#ifndef OTA_H
#define OTA_H

#include "ArduinoOTA.h"

// callbacks
typedef void(callback)();
typedef void(progressCallback)(unsigned int, unsigned int);
typedef void(errorCallback)(ota_error_t);

int otaStart();

void otaOnStart(callback * cb);
void otaOnEnd(callback * cb);
void otaOnProgress(progressCallback * cb);
void otaOnError(errorCallback * cb);

#endif
