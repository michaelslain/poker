#ifndef DOM_H
#define DOM_H

#include "object.h"

// DOM - Document Object Model (list of all objects in the scene)
typedef struct {
    Object** objects;
    int count;
    int capacity;
} DOM;

void DOM_Init(DOM* dom, int initialCapacity);
void DOM_AddObject(DOM* dom, Object* obj);
void DOM_RemoveObject(DOM* dom, Object* obj);
void DOM_Cleanup(DOM* dom);

#endif
