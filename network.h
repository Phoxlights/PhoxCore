#ifndef NETWORK_H
#define NETWORK_H

#include <IPAddress.h>

#define MAX_CONNECTION_ATTEMPTS 50
#define SSID_MAX 16
#define PASS_MAX 16
#define HOSTNAME_MAX 24

typedef enum {
    CONNECT = 0,
    CREATE = 1,
    OFF = 2
} NetworkMode;

// connect to an existing access point
int networkConnect(char * ssid, char * pass);
// create a wireless access point
int networkCreate(char * ssid, char * pass, IPAddress ip);
// stop AP, disconnect from network
int networkStop();
// get current IP address of wifi adapter
IPAddress networkGetIP();
// advertise ip using mdns
int networkAdvertise(char * hostname);

// save them powers
int networkOff();

bool networkIsConnected();

// TODO - reconnect strategy

#endif
