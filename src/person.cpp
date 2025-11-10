#include "person.hpp"
#include "debug.hpp"
#include "raymath.h"
#include "rlgl.h"

Person::Person(Vector3 pos, const std::string& personName, float personHeight)
    : Object(pos), inventory(), name(personName), height(personHeight), bodyYaw(0.0f),
      isSeated(false), seatPosition({0, 0, 0}) {
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
    // Uses height attribute to scale the mesh vertically
    
    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.x, 1, 0, 0);
    rlRotatef(bodyYaw * RAD2DEG, 0, 1, 0);  // Use bodyYaw for Y rotation
    rlRotatef(rotation.z, 0, 0, 1);
    rlScalef(scale.x, scale.y, scale.z);
    
    // Draw BODY (tall torso)
    rlPushMatrix();
    rlTranslatef(0.0f, 1.0f * height, 0.0f);
    DrawCubePitchBlack(0.55f, 1.4f * height, 0.35f);
    rlPopMatrix();
    
    // Draw NECK (connects head to body, overlapping top of torso)
    rlPushMatrix();
    rlTranslatef(0.0f, 1.75f * height, 0.0f);
    DrawCubePitchBlack(0.15f, 0.25f * height, 0.15f);
    rlPopMatrix();
    
    // Draw HEAD (sphere at top, overlapping neck)
    rlPushMatrix();
    rlTranslatef(0.0f, 2.1f * height, 0.0f);
    DrawSpherePitchBlack(0.3f, 10, 10);
    rlPopMatrix();
    
    // Draw SHOULDER connectors (make arms feel attached)
    rlPushMatrix();
    rlTranslatef(-0.35f, 1.55f * height, 0.0f);
    DrawCubePitchBlack(0.22f, 0.22f, 0.22f);
    rlPopMatrix();
    
    rlPushMatrix();
    rlTranslatef(0.35f, 1.55f * height, 0.0f);
    DrawCubePitchBlack(0.22f, 0.22f, 0.22f);
    rlPopMatrix();
    
    // Draw LEFT ARM (longer, overlapping shoulder)
    rlPushMatrix();
    rlTranslatef(-0.35f, 1.05f * height, 0.0f);
    DrawCubePitchBlack(0.17f, 1.1f * height, 0.17f);
    rlPopMatrix();
    
    // Draw RIGHT ARM (longer, overlapping shoulder)
    rlPushMatrix();
    rlTranslatef(0.35f, 1.05f * height, 0.0f);
    DrawCubePitchBlack(0.17f, 1.1f * height, 0.17f);
    rlPopMatrix();
    
    // Draw HIP/PELVIS connector (connects body to legs, overlapping bottom of torso)
    rlPushMatrix();
    rlTranslatef(0.0f, 0.25f * height, 0.0f);
    DrawCubePitchBlack(0.5f, 0.2f * height, 0.32f);
    rlPopMatrix();
    
    // Draw LEFT LEG (much taller, overlapping pelvis)
    rlPushMatrix();
    rlTranslatef(-0.15f, -0.5f * height, 0.0f);
    DrawCubePitchBlack(0.21f, 1.6f * height, 0.21f);
    rlPopMatrix();
    
    // Draw RIGHT LEG (much taller, overlapping pelvis)
    rlPushMatrix();
    rlTranslatef(0.15f, -0.5f * height, 0.0f);
    DrawCubePitchBlack(0.21f, 1.6f * height, 0.21f);
    rlPopMatrix();
    
    rlPopMatrix();
    
    // Reset color back to white so we don't affect other objects
    rlColor4ub(255, 255, 255, 255);
    
    // Draw collision cylinder wireframe if debug mode is on
    if (g_showCollisionDebug) {
        float topY = 2.4f * height;  // Top of head
        float radius = 0.5f;  // Hitbox radius
        DrawCylinderWires(position, radius, radius, topY, 16, LIME);
    }
}

std::string Person::GetType() const {
    return Object::GetType() + "_person";
}

void Person::SitDown(Vector3 seatPos) {
    isSeated = true;
    seatPosition = seatPos;
    position = seatPos;  // Move person to seat immediately
}

void Person::SitDownFacingPoint(Vector3 seatPos, Vector3 faceTowards) {
    isSeated = true;
    seatPosition = seatPos;
    position = seatPos;  // Move person to seat immediately
    
    // Calculate direction from seat to target point
    Vector3 direction = {
        faceTowards.x - seatPos.x,
        0.0f,  // Keep y at 0 for horizontal rotation only
        faceTowards.z - seatPos.z
    };
    
    // Calculate yaw angle to face the target
    bodyYaw = atan2f(direction.x, direction.z);
}

void Person::StandUp() {
    isSeated = false;
    // Position remains where the seat was, person can move freely from there
}
