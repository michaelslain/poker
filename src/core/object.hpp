#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "raylib.h"
#include <string>

class Object {
private:
    static int nextID;
    int id;

public:
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    bool usesLighting;  // Whether this object should be rendered with lighting shader (default: true)

    Object(Vector3 pos = {0.0f, 0.0f, 0.0f});
    virtual ~Object();

    virtual void Update(float deltaTime);
    virtual void Draw(Camera3D camera);
    virtual std::string GetType() const;
    
    // Clone this object at a new position (for spawning)
    virtual Object* Clone(Vector3 newPos) const;
    
    int GetID() const { return id; }
};

#endif
