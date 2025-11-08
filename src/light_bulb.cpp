#include "light_bulb.hpp"
#include "raymath.h"

LightBulb::LightBulb(Vector3 position, Color lightColor)
    : LightSource(position), color(lightColor)
{
    // Create point light (radial/omni-directional) using the static shader
    light = CreateLight(LIGHT_POINT, position, Vector3Zero(), lightColor, lightingShader);
}

LightBulb::~LightBulb() {
    // Nothing to clean up
}

void LightBulb::Update(float deltaTime) {
    (void)deltaTime;
    // Light position is updated via UpdateLight() called from main
}

void LightBulb::UpdateLight() {
    // Update light position in shader
    light.position = position;
    UpdateLightValues(lightingShader, light);
}

void LightBulb::Draw(Camera3D camera) {
    (void)camera;
    
    // Draw hanging chain/wire from ceiling
    Vector3 chainStart = position;
    chainStart.y += 0.5f;
    Vector3 chainEnd = position;
    chainEnd.y += 1.0f;
    DrawCylinder(chainStart, 0.02f, 0.02f, 0.5f, 6, (Color){60, 55, 50, 255});
    
    // Draw lantern top fixture (metal cap)
    Vector3 topFixture = position;
    topFixture.y += 0.45f;
    DrawCylinder(topFixture, 0.15f, 0.12f, 0.1f, 12, (Color){40, 35, 30, 255});
    
    // Draw lantern glass housing (semi-transparent)
    Vector3 housingPos = position;
    housingPos.y += 0.1f;
    DrawCylinder(housingPos, 0.25f, 0.25f, 0.5f, 12, (Color){80, 80, 70, 60});
    
    // Draw the actual bulb shape (upside-down teardrop)
    // Bulb bottom (wider sphere)
    Vector3 bulbBottom = position;
    bulbBottom.y -= 0.1f;
    DrawSphere(bulbBottom, 0.15f, (Color){255, 230, 180, 255});
    
    // Bulb middle
    DrawSphere(position, 0.12f, (Color){255, 230, 180, 255});
    
    // Bulb top (narrower, pointing up)
    Vector3 bulbTop = position;
    bulbTop.y += 0.1f;
    DrawSphere(bulbTop, 0.08f, (Color){255, 230, 180, 255});
    
    // Metal screw base at top of bulb
    Vector3 screwBase = position;
    screwBase.y += 0.18f;
    DrawCylinder(screwBase, 0.06f, 0.06f, 0.12f, 8, (Color){60, 55, 50, 255});
    
    // Draw layered glow halos for painterly effect
    DrawSphere(position, 0.28f, (Color){255, 200, 120, 120});
    DrawSphere(position, 0.38f, (Color){255, 180, 80, 70});
    DrawSphere(position, 0.48f, (Color){255, 160, 60, 40});
    DrawSphere(position, 0.58f, (Color){255, 140, 40, 20});
}

std::string LightBulb::GetType() const {
    return "light_bulb";
}
