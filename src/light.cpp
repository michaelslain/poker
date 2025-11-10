#include "light.hpp"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

// Initialize static members
Shader LightSource::lightingShader = { 0 };
bool LightSource::shaderInitialized = false;

LightSource::LightSource(Vector3 position) : Object(position) {
    usesLighting = false;  // Light sources render without lighting
}

LightSource::~LightSource() {
}

void LightSource::InitLightingSystem() {
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

void LightSource::CleanupLightingSystem() {
    if (!shaderInitialized) return;
    
    UnloadShader(lightingShader);
    shaderInitialized = false;
}

void LightSource::UpdateCameraPosition(Vector3 cameraPos) {
    if (!shaderInitialized) return;
    
    float cameraPosArray[3] = { cameraPos.x, cameraPos.y, cameraPos.z };
    SetShaderValue(lightingShader, lightingShader.locs[SHADER_LOC_VECTOR_VIEW], 
                   cameraPosArray, SHADER_UNIFORM_VEC3);
}
