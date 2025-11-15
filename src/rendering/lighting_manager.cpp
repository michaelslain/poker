#include "rendering/lighting_manager.hpp"

// Initialize static members
Shader LightingManager::lightingShader = { 0 };
bool LightingManager::shaderInitialized = false;
int LightingManager::lightsCount = 0;

void LightingManager::InitLightingSystem() {
    if (shaderInitialized) return;
    
    // Load lighting shader
    lightingShader = LoadShader("shaders/lighting.vs", "shaders/lighting.fs");
    
    // Validate shader loaded successfully
    if (lightingShader.id == 0) {
        TraceLog(LOG_ERROR, "LIGHT: Failed to load lighting shader!");
        return;
    }
    
    // Get shader locations
    lightingShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightingShader, "viewPos");
    
    // Set ambient light level (zero for pitch black except where lights reach)
    int ambientLoc = GetShaderLocation(lightingShader, "ambient");
    SetShaderValue(lightingShader, ambientLoc, (float[4]){0.0f, 0.0f, 0.0f, 1.0f}, SHADER_UNIFORM_VEC4);
    
    shaderInitialized = true;
}

void LightingManager::CleanupLightingSystem() {
    if (!shaderInitialized) return;
    
    UnloadShader(lightingShader);
    shaderInitialized = false;
}

void LightingManager::UpdateCameraPosition(Vector3 cameraPos) {
    if (!shaderInitialized) return;
    
    float cameraPosArray[3] = { cameraPos.x, cameraPos.y, cameraPos.z };
    SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW], 
                   cameraPosArray, SHADER_UNIFORM_VEC3);
}

// Create a light and get shader locations
RaylibLight LightingManager::CreateLight(int type, Vector3 position, Vector3 target, Color color) {
    RaylibLight light = { 0 };

    if (lightsCount < MAX_LIGHTS && shaderInitialized) {
        light.enabled = true;
        light.type = type;
        light.position = position;
        light.target = target;
        light.color = color;

        // NOTE: Lighting shader naming must be the provided ones
        light.enabledLoc = GetShaderLocation(lightingShader, TextFormat("lights[%i].enabled", lightsCount));
        light.typeLoc = GetShaderLocation(lightingShader, TextFormat("lights[%i].type", lightsCount));
        light.positionLoc = GetShaderLocation(lightingShader, TextFormat("lights[%i].position", lightsCount));
        light.targetLoc = GetShaderLocation(lightingShader, TextFormat("lights[%i].target", lightsCount));
        light.colorLoc = GetShaderLocation(lightingShader, TextFormat("lights[%i].color", lightsCount));

        UpdateLightValues(light);
        
        lightsCount++;
    }

    return light;
}

// Send light properties to shader
void LightingManager::UpdateLightValues(RaylibLight light) {
    if (!shaderInitialized) return;
    
    // Send to shader light enabled state and type
    SetShaderValue(lightingShader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(lightingShader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);

    // Send to shader light position values
    float position[3] = { light.position.x, light.position.y, light.position.z };
    SetShaderValue(lightingShader, light.positionLoc, position, SHADER_UNIFORM_VEC3);

    // Send to shader light target position values
    float target[3] = { light.target.x, light.target.y, light.target.z };
    SetShaderValue(lightingShader, light.targetLoc, target, SHADER_UNIFORM_VEC3);

    // Send to shader light color values
    float color[4] = { (float)light.color.r/(float)255, (float)light.color.g/(float)255, 
                       (float)light.color.b/(float)255, (float)light.color.a/(float)255 };
    SetShaderValue(lightingShader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
}
