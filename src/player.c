#include "player.h"
#include "raymath.h"
#include <math.h>

const char* Player_GetType(Object* obj) {
    (void)obj;
    return "player";
}

void Player_Init(Player* player, Vector3 pos) {
    Object_Init(&player->base, pos);
    
    // Set the getType function pointer
    player->base.getType = Player_GetType;
    
    // Initialize camera at eye level
    Vector3 eyePos = pos;
    eyePos.y += 1.7f;
    GameCamera_Init(&player->camera, eyePos);
    
    // Initialize inventory with capacity of 20
    Inventory_Init(&player->inventory, 20);
    
    player->speed = 5.0f;
    player->lookYaw = 0.0f;
    player->lookPitch = 0.0f;
}

void Player_Update(Player* player, float deltaTime) {
    // Mouse look
    Vector2 mouseDelta = GetMouseDelta();
    
    // Update player's look angles
    float sensitivity = 0.003f;
    player->lookYaw -= mouseDelta.x * sensitivity;
    player->lookPitch -= mouseDelta.y * sensitivity;
    
    // Clamp pitch
    if (player->lookPitch > 89.0f * DEG2RAD) player->lookPitch = 89.0f * DEG2RAD;
    if (player->lookPitch < -89.0f * DEG2RAD) player->lookPitch = -89.0f * DEG2RAD;
    
    // Calculate forward and right vectors from yaw (ignore pitch for movement)
    Vector3 forward = {
        sinf(player->lookYaw),
        0.0f,
        cosf(player->lookYaw)
    };
    
    Vector3 right = {
        sinf(player->lookYaw + PI/2),
        0.0f,
        cosf(player->lookYaw + PI/2)
    };
    
    // WASD movement
    Vector3 moveDir = {0.0f, 0.0f, 0.0f};
    
    if (IsKeyDown(KEY_W)) {
        moveDir = Vector3Add(moveDir, forward);
    }
    if (IsKeyDown(KEY_S)) {
        moveDir = Vector3Subtract(moveDir, forward);
    }
    if (IsKeyDown(KEY_D)) {
        moveDir = Vector3Subtract(moveDir, right);
    }
    if (IsKeyDown(KEY_A)) {
        moveDir = Vector3Add(moveDir, right);
    }
    
    // Normalize and apply speed
    if (Vector3Length(moveDir) > 0) {
        moveDir = Vector3Normalize(moveDir);
        moveDir = Vector3Scale(moveDir, player->speed * deltaTime);
        player->base.position = Vector3Add(player->base.position, moveDir);
    }
    
    // FOV adjustment
    GameCamera_AdjustFOV(&player->camera);
    
    // Update camera to follow player and match look direction
    player->camera.angle.x = player->lookPitch;
    player->camera.angle.y = player->lookYaw;
    
    // Position camera at player's eye level (add 1.7 to y for eye height)
    Vector3 eyePos = player->base.position;
    eyePos.y += 1.7f;
    GameCamera_SetTarget(&player->camera, eyePos);
    GameCamera_Update(&player->camera, (Vector2){0, 0}); // Already handled mouse delta above
}

void Player_Cleanup(Player* player) {
    Inventory_Cleanup(&player->inventory);
}

void Player_DrawInventoryUI(Player* player) {
    InventoryUI_Draw(&player->inventory);
}

Vector3 Player_GetPosition(Player* player) {
    return player->base.position;
}

Camera3D* Player_GetCamera(Player* player) {
    return &player->camera.camera;
}

Inventory* Player_GetInventory(Player* player) {
    return &player->inventory;
}
