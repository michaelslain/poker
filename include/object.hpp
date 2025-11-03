#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "raylib.h"

class Object {
private:
    static int nextID;
    int id;

public:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;

    Object(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Object();

    virtual void Update(float deltaTime);
    virtual void Draw(Camera3D camera);
    virtual const char* GetType() const;
    
    int GetID() const { return id; }
};

#endif
