#ifndef PSYCHEDELIC_MANAGER_HPP
#define PSYCHEDELIC_MANAGER_HPP

#include <raylib.h>

class PsychedelicManager {
private:
    static Shader psychedelicShader;
    static bool shaderInitialized;
    static int timeLoc;
    static int intensityLoc;
    
    static float tripStartTime;
    static float baseIntensity;
    static bool isTripping;
    
    static const float TRIP_DURATION; // 5 minutes

public:
    // Initialize the psychedelic shader system
    static void InitPsychedelicSystem();
    
    // Cleanup shader resources
    static void CleanupPsychedelicSystem();
    
    // Start a trip with given intensity (0.0 to 1.0)
    static void StartTrip(float intensity = 1.0f);
    
    // Stop the trip immediately
    static void StopTrip();
    
    // Update trip state (call every frame)
    static void Update(float deltaTime);
    
    // Check if currently tripping
    static bool IsTripping();
    
    // Get current effective intensity (accounts for stages)
    static float GetCurrentIntensity();
    
    // Get elapsed trip time
    static float GetTripTime();
    
    // Get the shader (returns reference to avoid copying)
    static Shader& GetPsychedelicShader();
    
    // Check if shader is initialized
    static bool IsInitialized();
};

#endif // PSYCHEDELIC_MANAGER_HPP
