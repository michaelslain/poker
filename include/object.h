#ifndef OBJECT_H
#define OBJECT_H

#include "raylib.h"

// Forward declare Object for function pointers
typedef struct Object Object;

// Virtual function pointers
typedef void (*UpdateFunc)(Object* self, float deltaTime);
typedef void (*DrawFunc)(Object* self, Camera3D camera);
typedef const char* (*GetTypeFunc)(Object* self);

typedef struct Object {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    
    // Virtual functions (can be NULL if not implemented)
    UpdateFunc update;
    DrawFunc draw;
    GetTypeFunc getType;  // Returns string identifier for this object type
    
    // Memory management flag
    bool isDynamicallyAllocated;  // true if malloc'd, false if stack-allocated
} Object;

void Object_Init(Object* obj, Vector3 pos);

#endif
