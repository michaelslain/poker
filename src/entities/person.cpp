#include "entities/person.hpp"
#include "core/debug.hpp"
#include "core/collision_categories.hpp"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

Person::Person(Vector3 pos, const std::string& personName, float personHeight)
    : Object(pos), inventory(), name(personName), height(personHeight), bodyYaw(0.0f),
      debugColor(BLACK),  // Default to black
      body(nullptr), geom(nullptr), physics(nullptr),
      isSeated(false), seatPosition({0, 0, 0}), standingYLevel(pos.y) {
    usesLighting = false;  // Persons render without lighting (pitch black)
    
    // Get physics from global instance
    physics = PhysicsWorld::GetGlobal();
    
    if (physics != nullptr) {
        // Calculate capsule dimensions based on person's height multiplier
        float actualCapsuleHeight = CAPSULE_HEIGHT * height;  // Scale by height multiplier
        float actualCapsuleOffset = actualCapsuleHeight / 2.0f;  // Half of actual height
        float radius = 0.4f;
        float cylinderLength = actualCapsuleHeight - (2.0f * radius);
        
        // Create dynamic body with mass for gravity
        body = dBodyCreate(physics->world);
        
        // NEW SYSTEM: position.y represents feet position (Y=0 at floor)
        // Body center should be at: feet + capsuleHeight/2
        float bodyCenterHeight = pos.y + actualCapsuleOffset;
        dBodySetPosition(body, pos.x, bodyCenterHeight, pos.z);
        
        TraceLog(LOG_INFO, "PERSON: Created '%s' (height=%.2fx) at feet pos (%.2f, %.2f, %.2f), body center at (%.2f, %.2f, %.2f), capsuleHeight=%.2f",
                 name.c_str(), height, pos.x, pos.y, pos.z, pos.x, bodyCenterHeight, pos.z, actualCapsuleHeight);

        // Set mass for the person (needed for gravity)
        dMass mass;
        dMassSetCapsuleTotal(&mass, 70.0f * height, 2, radius, cylinderLength); // Mass scales with height
        dBodySetMass(body, &mass);
        
        TraceLog(LOG_INFO, "PERSON: Capsule - radius=%.2f, cylinderLength=%.2f, totalHeight=%.2f, mass=%.1fkg",
                 radius, cylinderLength, actualCapsuleHeight, 70.0f * height);

        // Add damping to prevent oscillation with tall capsule
        dBodySetLinearDamping(body, 0.01);   // Minimal linear damping for natural falling
        dBodySetAngularDamping(body, 0.9);   // Very high angular damping to prevent rocking
        
        // Disable auto-disable so person doesn't "sleep" and fall through floor
        dBodySetAutoDisableFlag(body, 0);
        
        // Ensure zero initial velocity to prevent penetration on spawn
        dBodySetLinearVel(body, 0, 0, 0);
        dBodySetAngularVel(body, 0, 0, 0);

        // Create capsule geometry (ODE capsules are Z-axis aligned by default)
        geom = dCreateCapsule(physics->space, radius, cylinderLength);
        dGeomSetBody(geom, body);
        
        // Rotate capsule 90° around X-axis to align with Y-axis (stand upright)
        // ODE uses offset rotation for geometry relative to body
        dMatrix3 R;
        dRFromAxisAndAngle(R, 1, 0, 0, M_PI / 2.0);  // 90° around X-axis
        dGeomSetOffsetRotation(geom, R);

        // Set collision category: Person collides with WORLD, TABLES, and STAIRS only
        dGeomSetCategoryBits(geom, COLLISION_CATEGORY_PERSON);
        dGeomSetCollideBits(geom, COLLISION_MASK_PERSON);
        
        dGeomSetData(geom, this);
        
        // Debug: Verify physics state right after creation
        const dReal* verifyPos = dBodyGetPosition(body);
        const dReal* verifyVel = dBodyGetLinearVel(body);
        TraceLog(LOG_INFO, "PERSON: '%s' physics verified - BodyCenter=(%.2f, %.2f, %.2f), Velocity=(%.2f, %.2f, %.2f)",
                 name.c_str(), verifyPos[0], verifyPos[1], verifyPos[2], verifyVel[0], verifyVel[1], verifyVel[2]);
    } else {
        TraceLog(LOG_WARNING, "PERSON: '%s' created WITHOUT physics (PhysicsWorld::GetGlobal() returned nullptr)!", name.c_str());
    }
}

Person::~Person() {
    if (geom != nullptr) {
        dGeomDestroy(geom);
        geom = nullptr;
    }
    if (body != nullptr) {
        dBodyDestroy(body);
        body = nullptr;
    }
}

// Helper function to draw a cube using raw rlgl (no lighting)
static void DrawCubePitchBlack(float width, float height, float length, Color color) {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);

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
static void DrawSpherePitchBlack(float radius, int rings, int slices, Color color) {
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);

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

void Person::Update(float deltaTime) {
    (void)deltaTime;
    
    if (isSeated) {
        // NEW SYSTEM: When seated, seatPosition.y is the person's feet level
        // Body center is at feet + capsuleHeight/2
        float actualCapsuleOffset = (CAPSULE_HEIGHT * height) / 2.0f;
        float bodyCenterHeight = seatPosition.y + actualCapsuleOffset;
        
        if (body != nullptr) {
            dBodySetPosition(body, seatPosition.x, bodyCenterHeight, seatPosition.z);
            // Reset velocity when seated
            dBodySetLinearVel(body, 0, 0, 0);
            dBodySetAngularVel(body, 0, 0, 0);
        }
        if (geom != nullptr) {
            dGeomSetPosition(geom, seatPosition.x, bodyCenterHeight, seatPosition.z);
        }
        
        // Update Object position to match seat
        position = seatPosition;
    } else {
        // Not seated - read position from physics
        if (body != nullptr) {
            const dReal* physicsPos = dBodyGetPosition(body);
            
            // NEW SYSTEM: position.y represents feet position (Y=0 at floor)
            // Body center is at: feet + capsuleHeight/2
            // So: feet = bodyCenterHeight - capsuleHeight/2
            position.x = (float)physicsPos[0];
            position.z = (float)physicsPos[2];
            float actualCapsuleOffset = (CAPSULE_HEIGHT * height) / 2.0f;
            position.y = (float)physicsPos[1] - actualCapsuleOffset;  // Feet position
            
            // SAFETY NET: Prevent clipping through floor if physics fails
            // This should rarely/never trigger if collision is working correctly
            // Tolerance of -0.01 allows for minor floating point errors
            if (position.y < -0.01f) {
                TraceLog(LOG_ERROR, "PERSON '%s': PHYSICS FAILURE! Feet clipped through floor at Y=%.3f (body center=%.3f)",
                         name.c_str(), position.y, physicsPos[1]);
                
                // Emergency correction: teleport back to floor surface
                position.y = 0.0f;
                float correctedBodyY = 0.0f + actualCapsuleOffset;
                dBodySetPosition(body, position.x, correctedBodyY, position.z);
                dBodySetLinearVel(body, 0, 0, 0);  // Stop falling
                
                TraceLog(LOG_WARNING, "PERSON '%s': Emergency correction applied - teleported to floor", name.c_str());
            }

            #ifdef DEBUG_PERSON_PHYSICS
            // Debug: Check if position.y is clipping through floor (feet below ground)
            if (position.y < -0.1f) {
                TraceLog(LOG_WARNING, "PERSON '%s' (%s): CLIPPING! Feet at Y=%.2f (below floor at Y=0), physicsCenter=%.2f",
                         name.c_str(), GetType().c_str(), position.y, physicsPos[1]);
            }

            // Debug: Log position and velocity periodically
            static int frameCount = 0;
            frameCount++;
            if (frameCount % 300 == 0) {  // Every 5 seconds at 60fps
                TraceLog(LOG_DEBUG, "PERSON '%s' (%s): PhysCenter=(%.2f, %.2f, %.2f), Feet Y=%.2f, Vel=(%.2f, %.2f, %.2f)",
                         name.c_str(), GetType().c_str(),
                         physicsPos[0], physicsPos[1], physicsPos[2],
                         position.y,
                         linVel[0], linVel[1], linVel[2]);
            }
            #endif
        }
    }
}

void Person::Draw(Camera3D camera) {
    (void)camera;  // Unused parameter - persons don't use camera for rendering
    // Draw a humanoid figure with debug color
    // Uses height attribute to scale the mesh vertically

    rlPushMatrix();
    rlTranslatef(position.x, position.y, position.z);
    rlRotatef(rotation.x, 1, 0, 0);
    rlRotatef(bodyYaw * RAD2DEG, 0, 1, 0);  // Use bodyYaw for Y rotation
    rlRotatef(rotation.z, 0, 0, 1);
    rlScalef(scale.x, scale.y, scale.z);

    // NEW SYSTEM: position.y is at feet, so offset mesh upward by 1.3*height
    float meshOffset = 1.3f * height;

    // Draw BODY (tall torso)
    rlPushMatrix();
    rlTranslatef(0.0f, meshOffset + 1.0f * height, 0.0f);
    DrawCubePitchBlack(0.55f, 1.4f * height, 0.35f, debugColor);
    rlPopMatrix();

    // Draw NECK (connects head to body, overlapping top of torso)
    rlPushMatrix();
    rlTranslatef(0.0f, meshOffset + 1.75f * height, 0.0f);
    DrawCubePitchBlack(0.15f, 0.25f * height, 0.15f, debugColor);
    rlPopMatrix();

    // Draw HEAD (sphere at top, overlapping neck)
    rlPushMatrix();
    rlTranslatef(0.0f, meshOffset + 2.1f * height, 0.0f);
    DrawSpherePitchBlack(0.3f, 10, 10, debugColor);
    rlPopMatrix();

    // Draw SHOULDER connectors (make arms feel attached)
    rlPushMatrix();
    rlTranslatef(-0.35f, meshOffset + 1.55f * height, 0.0f);
    DrawCubePitchBlack(0.22f, 0.22f, 0.22f, debugColor);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(0.35f, meshOffset + 1.55f * height, 0.0f);
    DrawCubePitchBlack(0.22f, 0.22f, 0.22f, debugColor);
    rlPopMatrix();

    // Draw LEFT ARM (longer, overlapping shoulder)
    rlPushMatrix();
    rlTranslatef(-0.35f, meshOffset + 1.05f * height, 0.0f);
    DrawCubePitchBlack(0.17f, 1.1f * height, 0.17f, debugColor);
    rlPopMatrix();

    // Draw RIGHT ARM (longer, overlapping shoulder)
    rlPushMatrix();
    rlTranslatef(0.35f, meshOffset + 1.05f * height, 0.0f);
    DrawCubePitchBlack(0.17f, 1.1f * height, 0.17f, debugColor);
    rlPopMatrix();

    // Draw HIP/PELVIS connector (connects body to legs, overlapping bottom of torso)
    rlPushMatrix();
    rlTranslatef(0.0f, meshOffset + 0.25f * height, 0.0f);
    DrawCubePitchBlack(0.5f, 0.2f * height, 0.32f, debugColor);
    rlPopMatrix();

    // Draw LEFT LEG (much taller, overlapping pelvis)
    rlPushMatrix();
    rlTranslatef(-0.15f, meshOffset - 0.5f * height, 0.0f);
    DrawCubePitchBlack(0.21f, 1.6f * height, 0.21f, debugColor);
    rlPopMatrix();

    // Draw RIGHT LEG (much taller, overlapping pelvis)
    rlPushMatrix();
    rlTranslatef(0.15f, meshOffset - 0.5f * height, 0.0f);
    DrawCubePitchBlack(0.21f, 1.6f * height, 0.21f, debugColor);
    rlPopMatrix();

    rlPopMatrix();

    // Reset color back to white so we don't affect other objects
    rlColor4ub(255, 255, 255, 255);

    // Draw collision capsule wireframe showing actual ODE physics collider
    if (g_showCollisionDebug && body != nullptr) {
        const dReal* physicsPos = dBodyGetPosition(body);
        float actualCapsuleHeight = CAPSULE_HEIGHT * height;
        float radius = 0.4f;
        float cylinderLength = actualCapsuleHeight - (2.0f * radius);
        
        // Capsule center position
        Vector3 capsuleCenter = {(float)physicsPos[0], (float)physicsPos[1], (float)physicsPos[2]};
        
        // Bottom and top of cylinder part (excluding spheres)
        Vector3 cylinderBottom = {capsuleCenter.x, capsuleCenter.y - cylinderLength/2, capsuleCenter.z};
        Vector3 cylinderTop = {capsuleCenter.x, capsuleCenter.y + cylinderLength/2, capsuleCenter.z};
        
        // Draw cylinder part
        DrawCylinderWiresEx(cylinderBottom, cylinderTop, radius, radius, 8, GREEN);
        
        // Draw bottom hemisphere
        DrawSphereWires({capsuleCenter.x, capsuleCenter.y - cylinderLength/2, capsuleCenter.z}, radius, 8, 8, GREEN);
        
        // Draw top hemisphere  
        DrawSphereWires({capsuleCenter.x, capsuleCenter.y + cylinderLength/2, capsuleCenter.z}, radius, 8, 8, GREEN);
        
        // Draw ground position marker (where position.y should be)
        DrawSphereWires({position.x, position.y, position.z}, 0.1f, 4, 4, YELLOW);
    }
}

std::string Person::GetType() const {
    return Object::GetType() + "_person";
}

void Person::SitDown(Vector3 seatPos) {
    // Save current standing Y level before sitting
    standingYLevel = position.y;
    
    isSeated = true;
    seatPosition = seatPos;
    // Only change X and Z position, keep Y (height) the same
    position.x = seatPos.x;
    position.z = seatPos.z;
}

void Person::SitDownFacingPoint(Vector3 seatPos, Vector3 faceTowards) {
    // Save current standing Y level before sitting
    standingYLevel = position.y;
    
    isSeated = true;
    seatPosition = seatPos;
    // Only change X and Z position, keep Y (height) the same
    position.x = seatPos.x;
    position.z = seatPos.z;

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
    
    // Restore the saved standing Y level
    position.y = standingYLevel;
    
    // CRITICAL: Must update physics body to match restored position
    // Otherwise Update() will read wrong position from stale physics body
    if (body != nullptr) {
        // NEW SYSTEM: position.y is feet position, body center is at feet + capsuleHeight/2
        float actualCapsuleOffset = (CAPSULE_HEIGHT * height) / 2.0f;
        float bodyCenterHeight = position.y + actualCapsuleOffset;
        
        dBodySetPosition(body, position.x, bodyCenterHeight, position.z);
        
        if (geom != nullptr) {
            dGeomSetPosition(geom, position.x, bodyCenterHeight, position.z);
        }
        
        TraceLog(LOG_INFO, "PERSON '%s': Standing up, restored feet Y=%.2f, physics body at Y=%.2f",
                 name.c_str(), position.y, bodyCenterHeight);
    }
}
