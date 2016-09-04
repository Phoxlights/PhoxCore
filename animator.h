#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <transform.h>
#include <bitmap.h>

typedef struct AnimatorState* Animator;
typedef struct AnimatorLayerState* AnimatorLayer;
typedef struct AnimatorKeyframeState* AnimatorKeyframe;

// create an animator with given dimensions
Animator animatorCreate(int width, int height);

// clleaaaannnnn
int animatorFree(Animator state);

// start animation, yep.
int animatorPlay(Animator state);
// stops animation and clears buffer
int animatorStop(Animator state);
// stops animation but does not clear buffer
int animatorPause(Animator state);

// use animatorPlay to automatically tick animator
// at a specified interval, but if you need fine
// control you can call this directly instead
void animatorTick(Animator state);

// create and attach layer to animator
// TODO - layers should be able to exceed animator dimensions
AnimatorLayer animatorLayerCreate(Animator state, int width, int height, int index);
AnimatorLayer animatorGetLayerAt(Animator state, int index);
int animatorLayerStop(AnimatorLayerState * layerState);
int animatorLayerStart(AnimatorLayerState * layerState);
int animatorLayerUpdateBitmap(AnimatorLayerState * layerState, byte * buffer);

// create and attach keyframe to layer, optionally
// uses bitmap
AnimatorKeyframe animatorKeyframeCreate(AnimatorLayer layerState, int duration, Bitmap * bitmap);

// create an attach transform to keyframe
int animatorKeyframeAddTransform(AnimatorKeyframe keyframeState, Transform transform);

// get reference to animator's buffer
Bitmap * animatorGetBuffer(Animator state);

// a possibly easier to use api. just pushes to the
// last layer or keyframe in the stack
int animatorPushLayer(Animator state, int width, int height);
int animatorPushKeyframe(Animator state, int duration, Bitmap * bitmap);
int animatorPushTransform(Animator state, Transform transform);

#endif
