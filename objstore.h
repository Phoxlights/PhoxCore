#ifndef OBJSTORE_H
#define OBJSTORE_H

#define OBJSTORE_OBJ_NAME_LENGTH 16

int objStoreInit(int version);
int objStoreGet(char name[OBJSTORE_OBJ_NAME_LENGTH], int id, void * buffer, int length);
// NOTE - create returns the id of the newly created element
// NOTE - if create returns 0, thats bad yo!
int objStoreCreate(char name[OBJSTORE_OBJ_NAME_LENGTH], void * buffer, int length);
int objStoreUpdate(char name[OBJSTORE_OBJ_NAME_LENGTH], int id, void * buffer, int length);
int objStoreWipe(char name[OBJSTORE_OBJ_NAME_LENGTH]);
// TODO - list 

#endif
