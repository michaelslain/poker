#include "dom.h"
#include <stdlib.h>

#include "dom.h"
#include "raylib.h"
#include <stdlib.h>

// Global DOM instance
static DOM* g_dom = NULL;

void DOM_Init(DOM* dom, int initialCapacity) {
    dom->objects = (Object**)malloc(sizeof(Object*) * initialCapacity);
    if (dom->objects == NULL) {
        TraceLog(LOG_ERROR, "DOM_Init: malloc failed!");
        return;
    }
    dom->count = 0;
    dom->capacity = initialCapacity;
    TraceLog(LOG_INFO, "DOM_Init: Initialized with capacity %d", initialCapacity);
}

void DOM_AddObject(DOM* dom, Object* obj) {
    if (dom == NULL) {
        TraceLog(LOG_ERROR, "DOM_AddObject: dom is NULL!");
        return;
    }
    if (obj == NULL) {
        TraceLog(LOG_ERROR, "DOM_AddObject: obj is NULL!");
        return;
    }
    if (dom->objects == NULL) {
        TraceLog(LOG_ERROR, "DOM_AddObject: dom->objects is NULL!");
        return;
    }
    
    if (dom->count >= dom->capacity) {
        dom->capacity *= 2;
        dom->objects = (Object**)realloc(dom->objects, sizeof(Object*) * dom->capacity);
        if (dom->objects == NULL) {
            TraceLog(LOG_ERROR, "DOM_AddObject: realloc failed!");
            return;
        }
    }
    dom->objects[dom->count++] = obj;
}

void DOM_RemoveObject(DOM* dom, Object* obj) {
    for (int i = 0; i < dom->count; i++) {
        if (dom->objects[i] == obj) {
            // Shift all objects after this one down
            for (int j = i; j < dom->count - 1; j++) {
                dom->objects[j] = dom->objects[j + 1];
            }
            dom->count--;
            return;
        }
    }
}

void DOM_Cleanup(DOM* dom) {
    free(dom->objects);
    dom->objects = NULL;
    dom->count = 0;
    dom->capacity = 0;
}

void DOM_SetGlobal(DOM* dom) {
    g_dom = dom;
}

DOM* DOM_GetGlobal(void) {
    return g_dom;
}
