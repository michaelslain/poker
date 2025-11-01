#ifndef CHIP_H
#define CHIP_H

#include "item.h"
#include "rigidbody.h"
#include "physics.h"

typedef struct {
    Item base;
    int value;
    Color color;
    RigidBody rigidBody;
} Chip;

void Chip_Init(Chip* chip, int value, Vector3 pos, InteractCallback callback, PhysicsWorld* physics);
void Chip_Update(Chip* chip);
void Chip_Draw(Chip* chip, Camera3D camera);
void Chip_DrawIcon(Chip* chip, Rectangle destRect);
void Chip_Cleanup(Chip* chip);

// Get color based on value
Color Chip_GetColorFromValue(int value);

#endif
