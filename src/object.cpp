#include "object.hpp"

Object::Object(Vector3 pos)
    : parent(nullptr)
    , position(pos)
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

void Object::AddChild(Object* child) {
    if (child == nullptr) return;
    
    // Remove from previous parent if it has one
    if (child->parent != nullptr) {
        child->parent->RemoveChild(child);
    }
    
    children.push_back(child);
    child->parent = this;
}

void Object::RemoveChild(Object* child) {
    for (size_t i = 0; i < children.size(); i++) {
        if (children[i] == child) {
            children[i]->parent = nullptr;
            children.erase(children.begin() + i);
            return;
        }
    }
}

void Object::UpdateWithChildren(float deltaTime) {
    if (!isActive) return;
    
    // Update this object
    Update(deltaTime);
    
    // Update all children recursively
    for (size_t i = 0; i < children.size(); i++) {
        Object* child = children[i];
        if (child != nullptr) {
            child->UpdateWithChildren(deltaTime);
        }
    }
}

void Object::DrawWithChildren(Camera3D camera) {
    if (!isActive) return;
    
    // Draw this object
    Draw(camera);
    
    // Draw all children recursively
    // Use index-based iteration to be safer
    for (size_t i = 0; i < children.size(); i++) {
        Object* child = children[i];
        if (child != nullptr) {
            child->DrawWithChildren(camera);
        }
    }
}
