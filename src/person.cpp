#include "person.hpp"
#include "raymath.h"
#include "rlgl.h"

Person::Person(Vector3 pos, const std::string& personName)
    : Object(pos), name(personName) {
}

void Person::Draw(Camera3D camera) {
    // Draw a pitch black humanoid figure (head, body, arms, legs)
    // This is rendered with pure BLACK color and no lighting calculations
    
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.x, 1, 0, 0);
    rlRotatef(rotation.y, 0, 1, 0);
    rlRotatef(rotation.z, 0, 0, 1);
    rlScalef(scale.x, scale.y, scale.z);
    
    // Disable lighting to ensure pure black
    rlDisableBackfaceCulling();
    rlSetTexture(0);
    
    // Draw HEAD (sphere at top)
    rlPushMatrix();
    rlTranslatef(0.0f, 1.5f, 0.0f);
    DrawSphere({0, 0, 0}, 0.25f, BLACK);
    rlPopMatrix();
    
    // Draw BODY (box for torso)
    rlPushMatrix();
    rlTranslatef(0.0f, 0.7f, 0.0f);
    DrawCube({0, 0, 0}, 0.5f, 0.8f, 0.3f, BLACK);
    rlPopMatrix();
    
    // Draw LEFT ARM
    rlPushMatrix();
    rlTranslatef(-0.35f, 0.9f, 0.0f);
    DrawCube({0, 0, 0}, 0.15f, 0.7f, 0.15f, BLACK);
    rlPopMatrix();
    
    // Draw RIGHT ARM
    rlPushMatrix();
    rlTranslatef(0.35f, 0.9f, 0.0f);
    DrawCube({0, 0, 0}, 0.15f, 0.7f, 0.15f, BLACK);
    rlPopMatrix();
    
    // Draw LEFT LEG
    rlPushMatrix();
    rlTranslatef(-0.15f, -0.2f, 0.0f);
    DrawCube({0, 0, 0}, 0.18f, 0.9f, 0.18f, BLACK);
    rlPopMatrix();
    
    // Draw RIGHT LEG
    rlPushMatrix();
    rlTranslatef(0.15f, -0.2f, 0.0f);
    DrawCube({0, 0, 0}, 0.18f, 0.9f, 0.18f, BLACK);
    rlPopMatrix();
    
    rlEnableBackfaceCulling();
    rlPopMatrix();
}

const char* Person::GetType() const {
    return "person";
}
