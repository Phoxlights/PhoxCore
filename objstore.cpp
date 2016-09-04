#include <string.h>
#include "FS.h"
#include "objstore.h"

#define MAX_PATH_LENGTH 32

// magic 6 is 5 digits for max int and 1 digit for /
// example path "myobject/22415"
const int OBJSTORE_OBJ_PATH_LENGTH = OBJSTORE_OBJ_NAME_LENGTH + 6;

static int currentDBVer = 0;
static bool initialized = false;

typedef struct ObjStoreMetadata {
    int lastId;
} ObjStoreMetadata;

/*
 * SPIFFS specific implementation
 */
static char root[7];

static int spiffsInit(){
    snprintf(root, 7, "/v%03i/", currentDBVer);
    Serial.printf("set db root '%s'\n", root);

    if(!SPIFFS.begin()){
        Serial.println("Couldnt mount filesystem. Sucks.");
        return 0;
    }

    return 1;
}

static int getVPath(char * path, char * buffer){
    // TODO - ensure buffer is adequate length
    strcpy(buffer, root);
    strcat(buffer, path);
    return 1;
}

static int writeStruct(char * path, void * data, int length){
    char vpath[MAX_PATH_LENGTH];
    getVPath(path, vpath);

    File f = SPIFFS.open(vpath, "w+");
    if(!f){
        Serial.printf("couldn't create %s\n", vpath);
        return 0;
    }

    byte * d = (byte*)data;

    for(int i = 0; i < length; i++){
        f.write(&d[i], 1);
        //Serial.printf("%i,", d[i]);
    }
    //f.write((byte*)data, length);
    //Serial.println();
    f.close();

    return 1;
}

static int getStruct(char * path, void * data, int length){
    char vpath[MAX_PATH_LENGTH];
    getVPath(path, vpath);

    File f = SPIFFS.open(vpath, "r+");
    if(!f){
        Serial.printf("couldn't open %s\n", vpath);
        return 0;
    }

    byte * d = (byte*)data;

    for(int i = 0; i < length; i++){
        d[i] = (byte)f.read();
        //Serial.printf("%i,", d[i]);
    }
    //f.readBytes((byte*)data, length);
    //Serial.println();
    f.close();

    return 1;
}

static int createObjectPath(char name[OBJSTORE_OBJ_NAME_LENGTH], int id, char * buffer){
    if(sprintf(buffer, "%s/%i", name, id) < 0){
        return 0;
    }
    return 1;
}

static int getMetadata(char name[OBJSTORE_OBJ_NAME_LENGTH], ObjStoreMetadata * metadata){
    // TODO - cache metadata in memory?
    char objPath[OBJSTORE_OBJ_PATH_LENGTH];
    sprintf(objPath, "%s/meta", name);

    if(!getStruct(objPath, metadata, sizeof(ObjStoreMetadata))){
        Serial.println("couldnt get metadata");
        return 0;
    }

    return 1;
}
static int updateMetadata(char name[OBJSTORE_OBJ_NAME_LENGTH], ObjStoreMetadata * metadata){
    char objPath[OBJSTORE_OBJ_PATH_LENGTH];
    sprintf(objPath, "%s/meta", name);

    if(!writeStruct(objPath, metadata, sizeof(ObjStoreMetadata))){
        Serial.println("couldnt update metadata");
        return 0;
    }

    return 1;
}

int objStoreInit(int version){
    if(!version){
        // must include non-zero version number
        Serial.println("must pass in non-zero version number");
        return 0;
    }
    currentDBVer = version;

    if(!spiffsInit()){
        Serial.println("couldn't init spiffs stuff");
        return 0;
    }

    initialized = true;
    return 1;
}

int objStoreGet(char name[OBJSTORE_OBJ_NAME_LENGTH], int id, void * buffer, int length){
    if(!initialized){
        // init must be run first
        Serial.println("init has not been run");
        return 0;
    }

    char objPath[OBJSTORE_OBJ_PATH_LENGTH];
    if(!createObjectPath(name, id, objPath)){
        Serial.println("couldn't create object path");
        return 0;
    }

    if(!getStruct(objPath, buffer, length)){
        Serial.println("couldnt get data");
        return 0;
    }

    return 1;
}

int objStoreCreate(char name[OBJSTORE_OBJ_NAME_LENGTH], void * buffer, int length){
    if(!initialized){
        // init must be run first
        Serial.println("init has not been run");
        return 0;
    }

    int id = 0;

    ObjStoreMetadata metadata = {0};
    if(!getMetadata(name, &metadata)){
        // first time creating this object
        metadata.lastId = 1;
        id = 1;
    } else {
        metadata.lastId++;
        id = metadata.lastId;
    }

    char objPath[OBJSTORE_OBJ_PATH_LENGTH];
    if(!createObjectPath(name, id, objPath)){
        Serial.println("couldn't create object path");
        return 0;
    }

    if(!writeStruct(objPath, buffer, length)){
        Serial.println("couldnt write data");
        return 0;
    }

    if(!updateMetadata(name, &metadata)){
        Serial.println("couldnt update metadata. that's gonna sting later");
        return 0;
    }

    // if id is somehow still 0, thats not good
    return id;
}

int objStoreUpdate(char name[OBJSTORE_OBJ_NAME_LENGTH], int id, void * buffer, int length){
    if(!initialized){
        // init must be run first
        Serial.println("init has not been run");
        return 0;
    }

    // TODO - verify id doesnt exceed this objects
    // lastId

    char objPath[OBJSTORE_OBJ_PATH_LENGTH];
    if(!createObjectPath(name, id, objPath)){
        Serial.println("couldn't create object path");
        return 0;
    }

    if(!writeStruct(objPath, buffer, length)){
        Serial.println("couldnt write data");
        return 0;
    }

    return 1;
}

int objStoreWipe(char name[OBJSTORE_OBJ_PATH_LENGTH]){
    if(!initialized){
        // init must be run first
        Serial.println("init has not been run");
        return 0;
    }

    char vpath[MAX_PATH_LENGTH];
    getVPath(name, vpath);

    Dir dir = SPIFFS.openDir(vpath);
    while(dir.next()){
        // TODO - ensure dir.fileName directory name
        // is an exact match for `name`, not partial match
        if(!SPIFFS.remove(dir.fileName())){
            Serial.printf("Couldnt remove %s\n", dir.fileName().c_str());
        }
    }

    return 1;
}
