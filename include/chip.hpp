#ifndef CHIP_HPP
#define CHIP_HPP

#include "item.hpp"
#include "rigidbody.hpp"
#include "physics.hpp"

class Chip : public Item {
public:
    int value;
    Color color;
    RenderTexture2D iconTexture;
    bool iconTextureLoaded;
    RigidBody* rigidBody;

    Chip(int chipValue, Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Chip();

    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void DrawIcon(Rectangle destRect) override;
    std::string GetType() const override;
    
    static Color GetColorFromValue(int value);
};

#endif
