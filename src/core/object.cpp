#include "core/object.hpp"

// Static ID counter
int Object::nextID = 1;

Object::Object(Vector3 pos)
    : id(nextID++)
    , position(pos)
    , rotation({0.0f, 0.0f, 0.0f})
    , scale({1.0f, 1.0f, 1.0f})
    , usesLighting(true)  // Default: objects use lighting
{}

Object::~Object() {
    // Nothing to clean up - DOM manages lifetime
}

void Object::Update(float deltaTime) {
    (void)deltaTime;
    // Default: do nothing
}

void Object::Draw(Camera3D camera) {
    (void)camera;
    // Default: do nothing
}

std::string Object::GetType() const {
    return "object";
}

Object* Object::Clone(Vector3 newPos) const {
    // Default implementation: return nullptr (not cloneable)
    (void)newPos;
    return nullptr;
}
