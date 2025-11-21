#include "rendering/psychedelic_manager.hpp"
#include <raymath.h>

// Static member initialization
Shader PsychedelicManager::psychedelicShader = {0, nullptr};
bool PsychedelicManager::shaderInitialized = false;
int PsychedelicManager::timeLoc = -1;
int PsychedelicManager::intensityLoc = -1;
int PsychedelicManager::pinkHueLoc = -1;
int PsychedelicManager::tripTypeLoc = -1;

float PsychedelicManager::tripStartTime = 0.0f;
float PsychedelicManager::baseIntensity = 1.0f;
bool PsychedelicManager::isTripping = false;
TripType PsychedelicManager::currentTripType = TripType::SHROOMS;
bool PsychedelicManager::inComeDown = false;

const float PsychedelicManager::SHROOMS_DURATION = 300.0f; // 5 minutes
const float PsychedelicManager::MOLLY_DURATION = 600.0f;   // 10 minutes

void PsychedelicManager::InitPsychedelicSystem() {
    if (shaderInitialized) return;
    
    // Load psychedelic shader
    psychedelicShader = LoadShader("shaders/psychedelic.vs", "shaders/psychedelic.fs");
    
    // Get shader uniform locations
    timeLoc = GetShaderLocation(psychedelicShader, "time");
    intensityLoc = GetShaderLocation(psychedelicShader, "intensity");
    pinkHueLoc = GetShaderLocation(psychedelicShader, "pinkHue");
    tripTypeLoc = GetShaderLocation(psychedelicShader, "tripType");
    
    shaderInitialized = true;
}

void PsychedelicManager::CleanupPsychedelicSystem() {
    if (!shaderInitialized) return;
    
    UnloadShader(psychedelicShader);
    shaderInitialized = false;
}

void PsychedelicManager::StartTrip(float intensity, TripType type) {
    if (!shaderInitialized) return;
    
    isTripping = true;
    tripStartTime = 0.0f;
    baseIntensity = Clamp(intensity, 0.0f, 1.0f);
    currentTripType = type;
    inComeDown = false;
}

void PsychedelicManager::TriggerComeDown() {
    if (!isTripping || currentTripType != TripType::SALVIA) return;
    
    // Reset trip time to start come-down phase
    inComeDown = true;
    tripStartTime = 0.0f;
}

void PsychedelicManager::StopTrip() {
    isTripping = false;
    tripStartTime = 0.0f;
    inComeDown = false;
}

void PsychedelicManager::Update(float deltaTime) {
    if (!isTripping) return;
    
    tripStartTime += deltaTime;
    
    // Auto-stop shrooms trip after duration
    if (currentTripType == TripType::SHROOMS && tripStartTime >= SHROOMS_DURATION) {
        StopTrip();
    }
    
    // Auto-stop molly trip after duration
    if (currentTripType == TripType::MOLLY && tripStartTime >= MOLLY_DURATION) {
        StopTrip();
    }
    
    // Auto-stop Salvia come-down after fast transition
    if (currentTripType == TripType::SALVIA && inComeDown && tripStartTime >= 5.0f) {
        StopTrip();
    }
}

bool PsychedelicManager::IsTripping() {
    return isTripping;
}

TripType PsychedelicManager::GetTripType() {
    return currentTripType;
}

bool PsychedelicManager::IsInComeDown() {
    return inComeDown;
}

float PsychedelicManager::GetCurrentIntensity() {
    if (!isTripping) return 0.0f;
    
    if (currentTripType == TripType::SHROOMS) {
        // Shrooms: 5 minute trip with standard timing
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
            float comeDownProgress = (tripStartTime - peakEnd) / (SHROOMS_DURATION - peakEnd);
            intensity *= (1.0f - comeDownProgress);
        }
        
        return intensity;
    } 
    else if (currentTripType == TripType::SALVIA) {
        // Salvia: Fast come-up, level-long peak, fast come-down
        float intensity = baseIntensity;
        
        if (inComeDown) {
            // Fast come-down: 5 seconds
            float comeDownProgress = tripStartTime / 5.0f;
            intensity *= (1.0f - comeDownProgress);
        } else if (tripStartTime < 5.0f) {
            // Fast come-up: 5 seconds
            float stage = tripStartTime / 5.0f;
            intensity *= stage * stage; // Smooth ramp
        } else {
            // Peak: full intensity until level transition
            intensity *= 1.0f;
        }
        
        return intensity;
    }
    else if (currentTripType == TripType::MOLLY) {
        // Molly: 10 minute trip with gradual come-up/down, lower intensity
        float comeUpEnd = 90.0f;   // 1.5 minute come-up
        float peakEnd = 480.0f;     // 8 minute peak (1.5 + 6.5 + 2)
        
        float intensity = baseIntensity;
        
        if (tripStartTime < comeUpEnd) {
            // Come up: ramp 0 -> 1
            float stage = tripStartTime / comeUpEnd;
            intensity *= stage * stage; // Smooth ramp
        } else if (tripStartTime < peakEnd) {
            // Peak: full intensity with gentle waves
            float wave = sinf(tripStartTime * 0.3f) * 0.1f + 0.9f;
            intensity *= wave;
        } else {
            // Come down: ramp 1 -> 0
            float comeDownProgress = (tripStartTime - peakEnd) / (MOLLY_DURATION - peakEnd);
            intensity *= (1.0f - comeDownProgress);
        }
        
        return intensity;
    }
    
    return 0.0f;
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
