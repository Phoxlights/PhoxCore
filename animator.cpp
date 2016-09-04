#include <Ticker.h>
#include <animate.h>
#include <bitmap.h>
#include "animator.h"

// TODO - define interface (or at least some utils)
// for these guys because they are vuuurrrry similar
// TODO - each of these guys probably deserve their
// own files for readability

typedef struct AnimatorState {
    Bitmap * bitmap;
    AnimatorLayerState ** layers;
    Ticker * ticker;
} AnimatorState;

static int animatorLayerAddKeyframe(AnimatorLayerState * layerState, AnimatorKeyframeState * keyframeState);
static int animatorAddLayer(AnimatorState * state, AnimatorLayerState * layerState, int index);

typedef struct AnimatorKeyframeState {
    Keyframe * keyframe;
    Transform * transforms;
    int transformCount;
} AnimatorKeyframeState;

static int freeAnimatorKeyframeState(AnimatorKeyframeState * keyframeState){
    free(keyframeState->transforms);
    free(keyframeState);
    return 1;
}

AnimatorKeyframe animatorKeyframeCreate(AnimatorLayer layerState, int duration, Bitmap * bitmap){
    AnimatorKeyframeState * keyframeState = (AnimatorKeyframeState*)malloc(sizeof(AnimatorKeyframeState));
    if(bitmap != NULL){
        keyframeState->keyframe = Keyframe_create(duration, bitmap);
    } else {
        keyframeState->keyframe = Keyframe_create(duration);
    }

    // TODO - allocate less transforms?
    keyframeState->transforms = (Transform*)malloc(sizeof(Transform) * MAX_TRANSFORMS);
    keyframeState->transformCount = 0;

    if(!animatorLayerAddKeyframe(layerState, keyframeState)){
        Serial.println("ERROR: couldnt add keyframe");
        freeAnimatorKeyframeState(keyframeState);
        return NULL;
    }

    return keyframeState;
}

// NOTE - this guy is public
int animatorKeyframeAddTransform(AnimatorKeyframe keyframeState, Transform transform){
    if(keyframeState->transformCount >= MAX_TRANSFORMS){
        Serial.printf("ERROR: cant add transform, already at max: %i\n", MAX_TRANSFORMS);
        return 0;
    }
    Keyframe_add_transform(keyframeState->keyframe, transform);
    keyframeState->transforms[keyframeState->transformCount] = transform;
    keyframeState->transformCount++;
    return 1;
}







typedef struct AnimatorLayerState {
    Layer * layer;
    AnimatorKeyframeState ** keyframes;
    int keyframeCount;
} AnimatorLayerState;

static int animatorLayerFree(Animator state, AnimatorLayer layerState){
    // free keyframes
    for(int i = 0; i < layerState->keyframeCount; i++){
        freeAnimatorKeyframeState(layerState->keyframes[i]);
    }
    free(layerState->keyframes);

    // free layer
    Layer_free(layerState->layer);

    free(layerState);

    // find layer in layer list and NULL it out
    for(int i = 0; i < MAX_LAYERS; i++){
        if(state->layers[i] == layerState){
            state->layers[i] = NULL;
        }
    }
    return 1;
}

AnimatorLayer animatorLayerCreate(Animator state, int width, int height, int index){
    AnimatorLayerState * layerState = (AnimatorLayerState*)malloc(sizeof(AnimatorLayerState));
    layerState->layer = Layer_create(width, height);

    // TODO - allocate less keyframes?
    layerState->keyframes = (AnimatorKeyframeState**)malloc(sizeof(AnimatorKeyframeState*) * MAX_KEYFRAMES);
    layerState->keyframeCount = 0;

    if(!animatorAddLayer(state, layerState, index)){
        Serial.println("ERROR: couldnt add layer");
        animatorLayerFree(state, layerState);
        return NULL;
    }

    return layerState;
}

AnimatorLayer animatorGetLayerAt(Animator state, int index){
    if(index >= MAX_LAYERS){
        Serial.printf("cannot get layer at index %i. max layers is %i\n", index, MAX_LAYERS);
        return 0;
    }

    // NOTE - can return NULL
    return state->layers[index];
}

static int animatorLayerAddKeyframe(AnimatorLayerState * layerState, AnimatorKeyframeState * keyframeState){
    if(layerState->keyframeCount >= MAX_KEYFRAMES){
        Serial.printf("ERROR: cant add keyframe, already at max: %i\n", MAX_KEYFRAMES);
        return 0;
    }
    Layer_add_keyframe(layerState->layer, keyframeState->keyframe);
    layerState->keyframes[layerState->keyframeCount] = keyframeState;
    layerState->keyframeCount++;
    return 1;
}

static int animatorLayerTick(AnimatorLayerState * layerState, Bitmap * bitmap){
    Layer_tick(layerState->layer);
    Layer_composite(layerState->layer);
    // TODO - this leaks too much about layers
    Bitmap_blend(bitmap, layerState->layer->last_bitmap, ALPHA);
    return 1;
}

int animatorLayerStop(AnimatorLayerState * layerState){
    Layer_stop(layerState->layer);
    return 1;
}

int animatorLayerStart(AnimatorLayerState * layerState){
    Layer_play(layerState->layer);
    return 1;
}

int animatorLayerUpdateBitmap(AnimatorLayerState * layerState, byte * buffer){
    if(!Layer_updateBitmap(layerState->layer, buffer)){
        Serial.println("could not update layer bitmap");
        return 0;
    }
    return 1;
}






Animator animatorCreate(int width, int height){
    AnimatorState * state = (AnimatorState*)malloc(sizeof(AnimatorState));
    state->bitmap = Bitmap_create(width, height);

    // TODO - allocate less layers?
    state->layers = (AnimatorLayerState**)calloc(MAX_LAYERS, sizeof(AnimatorLayerState*));

    state->ticker = new Ticker();

    return state;
}

static int animatorAddLayer(AnimatorState * state, AnimatorLayerState * layerState, int index){
    if(index >= MAX_LAYERS){
        Serial.printf("cannot add layer at index %i. max layers is %i\n", index, MAX_LAYERS);
        return 0;
    }

    // if existing layer, free that mug
    if(state->layers[index] != NULL){
        animatorLayerFree(state, state->layers[index]);
    }

    state->layers[index] = layerState;
    return 1;
}

int animatorFree(Animator state){
    // free layers
    for(int i = 0; i < MAX_LAYERS; i++){
        if(state->layers[i] != NULL){
            animatorLayerFree(state, state->layers[i]);
        }
    }
    free(state->layers);

    Bitmap_free(state->bitmap);

    delete state->ticker;

    free(state);
    return 1;
}

Bitmap * animatorGetBuffer(Animator state){
    return state->bitmap;
}

void animatorTick(Animator state){
    byte empty[] = {0,0,0,0};
    Bitmap_fill(state->bitmap, empty);
    for(int i = 0; i < MAX_LAYERS; i++){
        if(state->layers[i] != NULL){
            animatorLayerTick(state->layers[i], state->bitmap);
        }
    }
}


static byte rgba_empty[4] = {0};

int animatorPlay(Animator state){
    // TODO - get tick rate from user
    state->ticker->attach_ms(30, animatorTick, state);
    return 1;
}
int animatorStop(Animator state){
    state->ticker->detach();
    // stop all layers
    for(int i = 0; i < MAX_LAYERS; i++){
        if(state->layers[i] != NULL){
            animatorLayerStop(state->layers[i]);
        }
    }
    Bitmap_fill(state->bitmap, rgba_empty);
    return 1;
}
int animatorPause(Animator state){
    state->ticker->detach();
    return 1;
}

// returns the index of the last initialized layer
static int getLastLayer(AnimatorState * state){
    for(int i = MAX_LAYERS-1; i >= 0; i++){
        if(state->layers[i] != NULL){
            return i;
        }
    }
    // couldn't find any allocated layers
    return -1;
}

int animatorPushLayer(Animator state, int width, int height){
    int lastLayer = getLastLayer(state);
    if(animatorLayerCreate(state, width, height, lastLayer+1) == NULL){
        Serial.printf("Couldn't create layer");
        return 0;
    }
    return 1;
}

int animatorPushKeyframe(Animator state, int duration, Bitmap * bitmap){
    int lastLayer = getLastLayer(state);
    if(lastLayer == -1){
        Serial.printf("ERROR: cant push keyframe because no layer exists");
        return 0;
    }
    AnimatorLayerState * layerState = state->layers[lastLayer];
    if(animatorKeyframeCreate(layerState, duration, bitmap) == NULL){
        Serial.printf("Couldn't create keyframe");
        return 0;
    }
    return 1;
}

int animatorPushTransform(Animator state, Transform transform){
    int lastLayer = getLastLayer(state);
    if(lastLayer == -1){
        Serial.printf("ERROR: cant push keyframe because no layer exists");
        return 0;
    }
    AnimatorLayerState * layerState = state->layers[lastLayer];
    if(layerState->keyframeCount == 0){
        Serial.printf("ERROR: cant push transform because no keyframe exists");
        return 0;
    }
    AnimatorKeyframeState * keyframeState = layerState->keyframes[layerState->keyframeCount-1];
    if(animatorKeyframeAddTransform(keyframeState, transform) == NULL){
        Serial.printf("Couldn't create transform");
        return 0;
    }
    return 1;
}
