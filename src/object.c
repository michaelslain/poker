#include "object.h"
#include <stddef.h>

void Object_Init(Object* obj, Vector3 pos) {
    obj->position = pos;
    obj->rotation = (Vector3){0.0f, 0.0f, 0.0f};
    obj->scale = (Vector3){1.0f, 1.0f, 1.0f};
    obj->update = NULL;  // Default: no update function
    obj->draw = NULL;    // Default: no draw function
    obj->getType = NULL; // Default: no type function
    obj->isDynamicallyAllocated = false;  // Default: stack-allocated
}
