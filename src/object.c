#include "object.h"

void Object_Init(Object* obj, Vector3 pos) {
    obj->position = pos;
    obj->rotation = (Vector3){0.0f, 0.0f, 0.0f};
    obj->scale = (Vector3){1.0f, 1.0f, 1.0f};
}
