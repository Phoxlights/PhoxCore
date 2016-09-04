#include <stdlib.h>
#include <Arduino.h>
#include "loop.h"

typedef struct LoopCallback {
    loopCallback func;
    void * state;
    int frequency;
    int nextTick;
    // TODO - once
    // TODO - name (for logging/debugging)
} LoopCallback;

typedef struct LoopCallbackNode {
    LoopCallback * cb;
    LoopCallbackNode * next;
} LoopCallbackNode;

LoopCallbackNode * nodeList = NULL;
LoopCallbackNode * nodeLast = NULL;

static int executeCallback(LoopCallback * cb){
    (cb->func)(cb->state);
    return 1;
}

static void freeLoopCallback(LoopCallback * cb){
    free(cb);
}

static void freeLoopCallbackNode(LoopCallbackNode * node){
    freeLoopCallback(node->cb);
    free(node);
}

LoopNode loopAttach(loopCallback cbFunc, int frequency, void * state){
    unsigned long time = millis();

    LoopCallback * cb = (LoopCallback*)malloc(sizeof(LoopCallback));
    cb->frequency = frequency;
    cb->func = cbFunc;
    cb->state = state;
    cb->nextTick = time + frequency;

    LoopCallbackNode * node = (LoopCallbackNode*)malloc(sizeof(LoopCallbackNode));
    node->next = NULL;
    node->cb = cb;

    // if this is the first list item
    if(nodeList == NULL){
        nodeList = node;
    // otherwise, add this to the last list item
    } else {
        nodeLast->next = node;
    }

    // this node is now the last node in the list
    nodeLast = node;

    return node;
}

int loopDetach(LoopNode node){
    LoopCallbackNode * curr = nodeList;
    LoopCallbackNode * prev = NULL;
    LoopCallback * cb;
    while(curr != NULL){
        // this is the guy were lookin for!
        if(curr == node){
            // if this is the first node in the list
            if(prev == NULL){
                // set the first item in the list to
                // this node's next item
                nodeList = curr->next;
            // if this is any other node in the list
            } else {
                // relink the list so this guy
                // can be removed
                prev->next = curr->next;
            }
            // if this is also the last node in the
            // list set a new last node
            if(curr == nodeLast){
                nodeLast = prev;
            }

            freeLoopCallbackNode(curr);
            return 1;
        }
        // next item in list
        prev = curr;
        curr = curr->next;
    }
    // couldnt find the guy i suppose
    return 0;
}

int loopTick(){
    unsigned long time = millis();
    LoopCallbackNode * curr = nodeList;
    LoopCallback * cb;
    while(curr != NULL){
        cb = curr->cb;

        // if frequency is 0 or timer has expired,
        // call the callback
        if(cb->frequency == 0 || time > cb->nextTick){
            if(!executeCallback(cb)){
                Serial.println("error executing callback");
            }
            // update nextTick time
            cb->nextTick = time + cb->frequency;
        }

        // next item in list
        curr = curr->next;
    }

    return 1;
}
