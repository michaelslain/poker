#include "person.hpp"
#include "raymath.h"
#include "rlgl.h"

Person::Person(Vector3 pos, const std::string& personName)
    : Object(pos), name(personName) {
}

// Helper function to draw a cube using raw rlgl (no lighting)
static void DrawCubePitchBlack(float width, float height, float length) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    rlBegin(RL_TRIANGLES);
    rlColor3f(0.0f, 0.0f, 0.0f); // Pure black

    // Front face
    rlVertex3f(x-width/2, y-height/2, z+length/2);
    rlVertex3f(x+width/2, y-height/2, z+length/2);
    rlVertex3f(x-width/2, y+height/2, z+length/2);
    rlVertex3f(x+width/2, y+height/2, z+length/2);
    rlVertex3f(x-width/2, y+height/2, z+length/2);
    rlVertex3f(x+width/2, y-height/2, z+length/2);

    // Back face
    rlVertex3f(x-width/2, y-height/2, z-length/2);
    rlVertex3f(x-width/2, y+height/2, z-length/2);
    rlVertex3f(x+width/2, y-height/2, z-length/2);
    rlVertex3f(x+width/2, y+height/2, z-length/2);
    rlVertex3f(x+width/2, y-height/2, z-length/2);
    rlVertex3f(x-width/2, y+height/2, z-length/2);

    // Top face
    rlVertex3f(x-width/2, y+height/2, z-length/2);
    rlVertex3f(x-width/2, y+height/2, z+length/2);
    rlVertex3f(x+width/2, y+height/2, z+length/2);
    rlVertex3f(x+width/2, y+height/2, z-length/2);
    rlVertex3f(x-width/2, y+height/2, z-length/2);
    rlVertex3f(x+width/2, y+height/2, z+length/2);

    // Bottom face
    rlVertex3f(x-width/2, y-height/2, z-length/2);
    rlVertex3f(x+width/2, y-height/2, z+length/2);
    rlVertex3f(x-width/2, y-height/2, z+length/2);
    rlVertex3f(x+width/2, y-height/2, z-length/2);
    rlVertex3f(x+width/2, y-height/2, z+length/2);
    rlVertex3f(x-width/2, y-height/2, z-length/2);

    // Right face
    rlVertex3f(x+width/2, y-height/2, z-length/2);
    rlVertex3f(x+width/2, y+height/2, z-length/2);
    rlVertex3f(x+width/2, y+height/2, z+length/2);
    rlVertex3f(x+width/2, y-height/2, z+length/2);
    rlVertex3f(x+width/2, y-height/2, z-length/2);
    rlVertex3f(x+width/2, y+height/2, z+length/2);

    // Left face
    rlVertex3f(x-width/2, y-height/2, z-length/2);
    rlVertex3f(x-width/2, y+height/2, z+length/2);
    rlVertex3f(x-width/2, y+height/2, z-length/2);
    rlVertex3f(x-width/2, y-height/2, z+length/2);
    rlVertex3f(x-width/2, y+height/2, z+length/2);
    rlVertex3f(x-width/2, y-height/2, z-length/2);

    rlEnd();
}

// Helper function to draw a sphere using raw rlgl (no lighting)
static void DrawSpherePitchBlack(float radius, int rings, int slices) {
    rlBegin(RL_TRIANGLES);
    rlColor3f(0.0f, 0.0f, 0.0f); // Pure black

    for (int i = 0; i < rings; i++) {
        for (int j = 0; j < slices; j++) {
            float theta1 = i * PI / rings;
            float theta2 = (i + 1) * PI / rings;
            float phi1 = j * 2 * PI / slices;
            float phi2 = (j + 1) * 2 * PI / slices;

            // First triangle
            rlVertex3f(radius * sinf(theta1) * cosf(phi1), radius * cosf(theta1), radius * sinf(theta1) * sinf(phi1));
            rlVertex3f(radius * sinf(theta2) * cosf(phi1), radius * cosf(theta2), radius * sinf(theta2) * sinf(phi1));
            rlVertex3f(radius * sinf(theta2) * cosf(phi2), radius * cosf(theta2), radius * sinf(theta2) * sinf(phi2));

            // Second triangle
            rlVertex3f(radius * sinf(theta1) * cosf(phi1), radius * cosf(theta1), radius * sinf(theta1) * sinf(phi1));
            rlVertex3f(radius * sinf(theta2) * cosf(phi2), radius * cosf(theta2), radius * sinf(theta2) * sinf(phi2));
            rlVertex3f(radius * sinf(theta1) * cosf(phi2), radius * cosf(theta1), radius * sinf(theta1) * sinf(phi2));
        }
    }

    rlEnd();
}

void Person::Draw(Camera3D camera) {
    // Draw a pitch black humanoid figure (head, body, arms, legs)
    // This is rendered with pure BLACK color (0,0,0) and no lighting calculations
    
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.x, 1, 0, 0);
    rlRotatef(rotation.y, 0, 1, 0);
    rlRotatef(rotation.z, 0, 0, 1);
    rlScalef(scale.x, scale.y, scale.z);
    
    // Draw HEAD (sphere at top)
    rlPushMatrix();
    rlTranslatef(0.0f, 1.5f, 0.0f);
    DrawSpherePitchBlack(0.25f, 8, 8);
    rlPopMatrix();
    
    // Draw BODY (box for torso)
    rlPushMatrix();
    rlTranslatef(0.0f, 0.7f, 0.0f);
    DrawCubePitchBlack(0.5f, 0.8f, 0.3f);
    rlPopMatrix();
    
    // Draw LEFT ARM
    rlPushMatrix();
    rlTranslatef(-0.35f, 0.9f, 0.0f);
    DrawCubePitchBlack(0.15f, 0.7f, 0.15f);
    rlPopMatrix();
    
    // Draw RIGHT ARM
    rlPushMatrix();
    rlTranslatef(0.35f, 0.9f, 0.0f);
    DrawCubePitchBlack(0.15f, 0.7f, 0.15f);
    rlPopMatrix();
    
    // Draw LEFT LEG
    rlPushMatrix();
    rlTranslatef(-0.15f, -0.2f, 0.0f);
    DrawCubePitchBlack(0.18f, 0.9f, 0.18f);
    rlPopMatrix();
    
    // Draw RIGHT LEG
    rlPushMatrix();
    rlTranslatef(0.15f, -0.2f, 0.0f);
    DrawCubePitchBlack(0.18f, 0.9f, 0.18f);
    rlPopMatrix();
    
    rlPopMatrix();
    
    // Reset color back to white so we don't affect other objects
    rlColor4ub(255, 255, 255, 255);
}

const char* Person::GetType() const {
    return "person";
}
