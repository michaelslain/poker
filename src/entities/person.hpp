#ifndef PERSON_HPP
#define PERSON_HPP

#include "raylib.h"
#include "core/object.hpp"
#include "items/inventory.hpp"
#include "core/physics.hpp"
#include "core/collision_categories.hpp"
#include <string>
#include <ode/ode.h>

class Person : public Object {
protected:
    Inventory inventory;
    std::string name;
    float height;  // Height multiplier for the mesh (1.0 = normal, >1.0 = taller)
    float bodyYaw;  // Body rotation in radians (Y-axis rotation)
    Color debugColor;  // Debug rendering color (default: black)
    
    // Physics
    dBodyID body;           // ODE physics body
    dGeomID geom;           // ODE collision geometry (capsule)
    PhysicsWorld* physics;  // Reference to physics world
    
    // Physics constants
    static constexpr float STANDING_HEIGHT = 1.8f;        // Person eye height (camera height)
    static constexpr float CAPSULE_HEIGHT = 3.4f;         // Total capsule height (matches full visual mesh)
    static constexpr float CAPSULE_OFFSET = CAPSULE_HEIGHT / 2.0f;  // Offset from body center to feet
    
    // Seating
    bool isSeated;          // Whether person is seated at a table
    Vector3 seatPosition;   // Position where person is seated
    float standingYLevel;   // Y position to return to when standing up (saved before sitting)

public:
    Person(Vector3 pos, const std::string& personName, float personHeight = 1.0f);
    virtual ~Person();
    
    // Override Update to sync with physics
    void Update(float deltaTime) override;
    
    // Override Draw to render pitch black (unaffected by lighting)
    void Draw(Camera3D camera) override;
    
    // Seating methods
    virtual void SitDown(Vector3 seatPos);
    virtual void SitDownFacingPoint(Vector3 seatPos, Vector3 faceTowards);
    virtual void StandUp();
    bool IsSeated() const { return isSeated; }
    
    // Betting (to be overridden by Player and Enemy)
    // Returns: 0=fold, 1=call, 2=raise
    // raiseAmount is set if action is raise
    virtual int PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount) {
        (void)currentBet; (void)callAmount; (void)minRaise; (void)maxRaise; (void)raiseAmount;
        return 0; // Default: fold
    }
    
    // Body rotation
    void SetBodyYaw(float yaw) { bodyYaw = yaw; }
    float GetBodyYaw() const { return bodyYaw; }
    
    // Accessors
    Inventory* GetInventory() { return &inventory; }
    const std::string& GetName() const { return name; }
    void SetName(const std::string& newName) { name = newName; }
    float GetHeight() const { return height; }
    void SetHeight(float newHeight) { height = newHeight; }
    dBodyID GetBody() const { return body; }
    dGeomID GetGeom() const { return geom; }
    
    std::string GetType() const override;
};

#endif
