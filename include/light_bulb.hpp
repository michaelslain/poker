#ifndef LIGHT_BULB_HPP
#define LIGHT_BULB_HPP

#include "light.hpp"
#include "raylib.h"
#include "rlights.h"

class LightBulb : public LightSource {
private:
    Light light;  // raylib Light struct
    Color color;

public:
    LightBulb(Vector3 position, Color lightColor);
    virtual ~LightBulb();

    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    const char* GetType() const override;
    
    // Override LightSource's pure virtual function
    void UpdateLight() override;
    
    // Accessor
    Light GetLight() const { return light; }
};

#endif
