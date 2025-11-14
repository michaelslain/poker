#ifndef SUBSTANCE_HPP
#define SUBSTANCE_HPP

#include "item.hpp"
#include "rigidbody.hpp"
#include "physics.hpp"

class Substance : public Item {
protected:
    RigidBody* rigidBody;
    Color color;  // Visual color of the substance

public:
    Substance(Vector3 pos, Color substanceColor, PhysicsWorld* physics = nullptr);
    virtual ~Substance();

    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void DrawIcon(Rectangle destRect) override;
    std::string GetType() const override;
    Object* Clone(Vector3 newPos) const override = 0;  // Pure virtual - subclasses must specify concrete type

    // Substance-specific methods
    virtual void Consume() = 0;  // Pure virtual - each substance has unique effects
    virtual const char* GetName() const = 0;  // Pure virtual - each substance has a name
};

#endif
