#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "raylib.h"
#include <vector>

class Object {
private:
    std::vector<Object*> children;
    Object* parent;

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
    
    // Children management
    void AddChild(Object* child);
    void RemoveChild(Object* child);
    const std::vector<Object*>& GetChildren() const { return children; }
    Object* GetParent() const { return parent; }
    
    // Recursive update/draw for children
    void UpdateWithChildren(float deltaTime);
    void DrawWithChildren(Camera3D camera);
    
    // Mark object for deletion (will be cleaned up by DOM on next frame)
    void DeleteSelf() { isActive = false; }
};

#endif
