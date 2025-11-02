#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "object.hpp"
#include "raylib.h"

class LightSource : public Object {
protected:
    static Shader lightingShader;
    static bool shaderInitialized;

public:
    LightSource(Vector3 position);
    virtual ~LightSource();

    // Static methods to manage the lighting shader
    static void InitLightingSystem();
    static void CleanupLightingSystem();
    static Shader GetLightingShader() { return lightingShader; }
    static void UpdateCameraPosition(Vector3 cameraPos);
    
    // Virtual method for updating light in shader
    virtual void UpdateLight() = 0;
};

#endif
