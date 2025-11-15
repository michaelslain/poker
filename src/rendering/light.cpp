#include "rendering/light.hpp"

Light::Light(Vector3 position) : Object(position) {
    usesLighting = false;  // Light sources render without lighting
}

Light::~Light() {
}

std::string Light::GetType() const {
    return Object::GetType() + "_light";
}
