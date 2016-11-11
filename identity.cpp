#include "identity.h"

bool identityIsDefined(Identity * id){
    return (id->model + id->serial + id->bin + id->eventVer + id->dbVer) > 0;
}

int identityCopy(Identity * target, Identity * src){
    target->model = src->model;
    target->serial = src->serial;
    target->bin = src->bin;
    target->eventVer = src->eventVer;
    target->dbVer = src->dbVer;
    return 0;
}
