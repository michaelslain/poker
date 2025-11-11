#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "object.hpp"
#include "raylib.h"

// Base class for all light objects in the scene
// This is a marker class that provides common functionality for all lights
class Light : public Object {
public:
    Light(Vector3 position);
    virtual ~Light();
    
    // Virtual method for updating light in shader (can be overridden by derived classes)
    virtual void UpdateLight() {}
    
    // Override GetType to return hierarchical type
    std::string GetType() const override;
};

#endif
