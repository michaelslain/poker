#include "rendering/psychedelic_manager.hpp"
#include <raymath.h>

// Static member initialization
Shader PsychedelicManager::psychedelicShader = {0, nullptr};
bool PsychedelicManager::shaderInitialized = false;
int PsychedelicManager::timeLoc = -1;
int PsychedelicManager::intensityLoc = -1;

float PsychedelicManager::tripStartTime = 0.0f;
float PsychedelicManager::baseIntensity = 1.0f;
bool PsychedelicManager::isTripping = false;

const float PsychedelicManager::TRIP_DURATION = 300.0f; // 5 minutes

void PsychedelicManager::InitPsychedelicSystem() {
    if (shaderInitialized) return;
    
    // Load psychedelic shader
    psychedelicShader = LoadShader("shaders/psychedelic.vs", "shaders/psychedelic.fs");
    
    // Get shader uniform locations
    timeLoc = GetShaderLocation(psychedelicShader, "time");
    intensityLoc = GetShaderLocation(psychedelicShader, "intensity");
    
    shaderInitialized = true;
}

void PsychedelicManager::CleanupPsychedelicSystem() {
    if (!shaderInitialized) return;
    
    UnloadShader(psychedelicShader);
    shaderInitialized = false;
}

void PsychedelicManager::StartTrip(float intensity) {
    if (!shaderInitialized) return;
    
    isTripping = true;
    tripStartTime = 0.0f;
    baseIntensity = Clamp(intensity, 0.0f, 1.0f);
}

void PsychedelicManager::StopTrip() {
    isTripping = false;
    tripStartTime = 0.0f;
}

void PsychedelicManager::Update(float deltaTime) {
    if (!isTripping) return;
    
    tripStartTime += deltaTime;
    
    // Auto-stop trip after duration
    if (tripStartTime >= TRIP_DURATION) {
        StopTrip();
    }
}

bool PsychedelicManager::IsTripping() {
    return isTripping;
}

float PsychedelicManager::GetCurrentIntensity() {
    if (!isTripping) return 0.0f;
    
    // Calculate intensity based on trip stage
    float comeUpEnd = 60.0f;
    float peakEnd = 180.0f;
    
    float intensity = baseIntensity;
    
    if (tripStartTime < comeUpEnd) {
        // Come up: ramp 0 -> 1
        float stage = tripStartTime / comeUpEnd;
        intensity *= stage * stage; // Smooth ramp
    } else if (tripStartTime < peakEnd) {
        // Peak: full intensity with waves
        float wave = sinf(tripStartTime * 0.5f) * 0.15f + 0.85f;
        intensity *= wave;
    } else {
        // Come down: ramp 1 -> 0
        float comeDownProgress = (tripStartTime - peakEnd) / (TRIP_DURATION - peakEnd);
        intensity *= (1.0f - comeDownProgress);
    }
    
    return intensity;
}

float PsychedelicManager::GetTripTime() {
    return tripStartTime;
}

Shader& PsychedelicManager::GetPsychedelicShader() {
    return psychedelicShader;
}

bool PsychedelicManager::IsInitialized() {
    return shaderInitialized;
}
