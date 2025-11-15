#ifndef LIGHT_BULB_HPP
#define LIGHT_BULB_HPP

#include "rendering/light.hpp"
#include "raylib.h"

class LightBulb : public Light {
private:
    void* raylibLightPtr;  // Opaque pointer to raylib Light struct

public:
    LightBulb(Vector3 position, Color lightColor);
    virtual ~LightBulb();

    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    std::string GetType() const override;
    
    // Override Light's pure virtual function
    void UpdateLight() override;
};

#endif
