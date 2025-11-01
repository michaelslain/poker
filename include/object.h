#ifndef OBJECT_H
#define OBJECT_H

#include "raylib.h"

typedef struct {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
} Object;

void Object_Init(Object* obj, Vector3 pos);

#endif
