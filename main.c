#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "camera.h"
#include "interactable.h"
#include "card.h"
#include "plane.h"
#include "physics.h"
#include <stdlib.h>
#include <stddef.h>

// DOM - Document Object Model (list of all objects in the scene)
typedef struct {
    Object** objects;
    int count;
    int capacity;
} DOM;

void DOM_Init(DOM* dom, int initialCapacity) {
    dom->objects = (Object**)malloc(sizeof(Object*) * initialCapacity);
    dom->count = 0;
    dom->capacity = initialCapacity;
}

void DOM_AddObject(DOM* dom, Object* obj) {
    if (dom->count >= dom->capacity) {
        dom->capacity *= 2;
        dom->objects = (Object**)realloc(dom->objects, sizeof(Object*) * dom->capacity);
    }
    dom->objects[dom->count++] = obj;
}

void DOM_RemoveObject(DOM* dom, Object* obj) {
    for (int i = 0; i < dom->count; i++) {
        if (dom->objects[i] == obj) {
            // Shift all objects after this one down
            for (int j = i; j < dom->count - 1; j++) {
                dom->objects[j] = dom->objects[j + 1];
            }
            dom->count--;
            return;
        }
    }
}

void DOM_Cleanup(DOM* dom) {
    free(dom->objects);
    dom->objects = NULL;
    dom->count = 0;
    dom->capacity = 0;
}

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Poker - First Person");
    
    // Disable cursor for FPS controls
    DisableCursor();
    
    // Initialize physics world
    PhysicsWorld physics;
    Physics_Init(&physics);
    
    // Initialize DOM
    DOM dom;
    DOM_Init(&dom, 50);
    
    // Initialize player (camera is initialized inside player)
    Player player;
    Player_Init(&player, (Vector3){0.0f, 0.0f, 0.0f});
    DOM_AddObject(&dom, (Object*)&player);
    
    // Initialize ground plane with physics
    Plane groundPlane;
    Plane_Init(&groundPlane, (Vector3){0.0f, 0.0f, 0.0f}, (Vector2){50.0f, 50.0f}, LIGHTGRAY, &physics);
    DOM_AddObject(&dom, (Object*)&groundPlane);
    
    // Create some card objects in the world (dynamic allocation)
    // Start them higher up so they fall down
    int cardCount = 5;
    Card* cards = (Card*)malloc(sizeof(Card) * cardCount);
    
    Card_Init(&cards[0], SUIT_SPADES, RANK_ACE, (Vector3){3.0f, 5.0f, 0.0f}, NULL, &physics);
    Card_Init(&cards[1], SUIT_HEARTS, RANK_KING, (Vector3){-3.0f, 6.0f, 2.0f}, NULL, &physics);
    Card_Init(&cards[2], SUIT_DIAMONDS, RANK_QUEEN, (Vector3){0.0f, 7.0f, -5.0f}, NULL, &physics);
    Card_Init(&cards[3], SUIT_CLUBS, RANK_TEN, (Vector3){5.0f, 8.0f, 5.0f}, NULL, &physics);
    Card_Init(&cards[4], SUIT_HEARTS, RANK_SEVEN, (Vector3){-2.0f, 9.0f, -3.0f}, NULL, &physics);
    
    // Add cards to DOM
    for (int i = 0; i < cardCount; i++) {
        DOM_AddObject(&dom, (Object*)&cards[i]);
    }

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        // Toggle cursor lock with U key
        if (IsKeyPressed(KEY_U)) {
            if (IsCursorHidden()) {
                EnableCursor();
            } else {
                DisableCursor();
            }
        }
        
        // Update player (handles input, camera, and FOV)
        Player_Update(&player, deltaTime);
        
        // Step physics simulation
        Physics_Step(&physics, deltaTime);
        
        // Update all cards (sync physics to object positions)
        for (int i = 0; i < cardCount; i++) {
            Card_Update(&cards[i]);
        }
        
        // Find closest card
        int closestIndex = -1;
        float closestDistance = 999999.0f;
        Vector3 playerPos = Player_GetPosition(&player);
        
        for (int i = 0; i < cardCount; i++) {
            if (!cards[i].base.base.isActive) continue;
            
            float dist = Vector3Distance(playerPos, cards[i].base.base.base.position);
            
            if (dist <= cards[i].base.base.interactRange && dist < closestDistance) {
                closestDistance = dist;
                closestIndex = i;
            }
        }
        
        // Interact with closest card on E press
        if (closestIndex != -1 && IsKeyPressed(KEY_E)) {
            // Add card to player's inventory
            Inventory* inventory = Player_GetInventory(&player);
            Inventory_AddItem(inventory, &cards[closestIndex].base);
            
            // Remove the card from the DOM
            DOM_RemoveObject(&dom, (Object*)&cards[closestIndex]);
            
            // Deactivate the card from the world
            cards[closestIndex].base.base.isActive = false;
            
            TraceLog(LOG_INFO, "Card picked up! Inventory count: %d, DOM count: %d", 
                     inventory->count, dom.count);
        }
        
        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(*Player_GetCamera(&player));
                // Draw ground plane
                Plane_Draw(&groundPlane);
                DrawGrid(50, 1.0f);
                
                // Draw cards
                Camera3D camera = *Player_GetCamera(&player);
                for (int i = 0; i < cardCount; i++) {
                    bool isClosest = (i == closestIndex);
                    Card_Draw(&cards[i], isClosest, camera);
                }
                
            EndMode3D();
            
            // Draw inventory UI
            Player_DrawInventoryUI(&player);
            
            // Draw UI
            if (closestIndex != -1) {
                DrawText("Press E to interact", screenWidth / 2 - 80, screenHeight - 40, 20, GREEN);
            }
            
            DrawFPS(10, screenHeight - 30);

        EndDrawing();
    }

    // De-Initialization
    for (int i = 0; i < cardCount; i++) {
        Card_Cleanup(&cards[i]);
    }
    free(cards);
    Player_Cleanup(&player);
    Plane_Cleanup(&groundPlane);
    DOM_Cleanup(&dom);
    Physics_Cleanup(&physics);
    CloseWindow();

    return 0;
}
