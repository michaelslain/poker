#include "object.hpp"

Object::Object(Vector3 pos)
    : position(pos)
    , rotation({0.0f, 0.0f, 0.0f})
    , scale({1.0f, 1.0f, 1.0f})
    , isActive(true)
    , isDynamicallyAllocated(false)
{
}

void Object::Update(float deltaTime) {
    (void)deltaTime;
    // Default: do nothing
}

void Object::Draw(Camera3D camera) {
    (void)camera;
    // Default: do nothing
}

const char* Object::GetType() const {
    return "object";
}
