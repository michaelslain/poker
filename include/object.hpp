#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "raylib.h"

class Object {
public:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    bool isActive;
    bool isDynamicallyAllocated;

    Object(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Object() = default;

    virtual void Update(float deltaTime);
    virtual void Draw(Camera3D camera);
    virtual const char* GetType() const;
};

#endif
