#ifndef LIGHTING_MANAGER_HPP
#define LIGHTING_MANAGER_HPP

#include "raylib.h"

// Max dynamic lights supported by shader
#define MAX_LIGHTS 4

// Light type enum
typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT
} LightType;

// Light data structure (raylib light)
typedef struct {
    int type;
    bool enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    float attenuation;
    
    // Shader locations
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int attenuationLoc;
} RaylibLight;

// Static utility class for managing the global lighting shader system
class LightingManager {
private:
    static Shader lightingShader;
    static bool shaderInitialized;
    static int lightsCount;

public:
    // Static methods to manage the lighting shader
    static void InitLightingSystem();
    static void CleanupLightingSystem();
    static Shader& GetLightingShader() { return lightingShader; }
    static void UpdateCameraPosition(Vector3 cameraPos);
    
    // Light management functions (moved from rlights.h)
    static RaylibLight CreateLight(int type, Vector3 position, Vector3 target, Color color);
    static void UpdateLightValues(RaylibLight light);
};

#endif
