#ifndef LOOP_H
#define LOOP_H

typedef struct LoopCallbackNode * LoopNode;
typedef void(*loopCallback)(void *state);

LoopNode loopAttach(loopCallback cb, int frequency, void * state);
int loopDetach(LoopNode node);
int loopTick();

#endif
