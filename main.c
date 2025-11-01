#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include "camera.h"
#include "interactable.h"
#include "card.h"
#include <stdlib.h>
#include <stddef.h>

int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Poker - First Person");
    
    // Disable cursor for FPS controls
    DisableCursor();
    
    // Initialize player (camera is initialized inside player)
    Player player;
    Player_Init(&player, (Vector3){0.0f, 0.0f, 0.0f});
    
    // Create some card objects in the world (dynamic allocation)
    int cardCount = 5;
    int cardCapacity = 10;
    Card* cards = (Card*)malloc(sizeof(Card) * cardCapacity);
    
    Card_Init(&cards[0], SUIT_SPADES, RANK_ACE, (Vector3){3.0f, 0.5f, 0.0f}, NULL);
    Card_Init(&cards[1], SUIT_HEARTS, RANK_KING, (Vector3){-3.0f, 0.5f, 2.0f}, NULL);
    Card_Init(&cards[2], SUIT_DIAMONDS, RANK_QUEEN, (Vector3){0.0f, 0.5f, -5.0f}, NULL);
    Card_Init(&cards[3], SUIT_CLUBS, RANK_TEN, (Vector3){5.0f, 0.5f, 5.0f}, NULL);
    Card_Init(&cards[4], SUIT_HEARTS, RANK_SEVEN, (Vector3){-2.0f, 0.5f, -3.0f}, NULL);

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
        
        // Find closest card
        int closestIndex = -1;
        float closestDistance = 999999.0f;
        Vector3 playerPos = Player_GetPosition(&player);
        
        for (int i = 0; i < cardCount; i++) {
            if (!cards[i].base.isActive) continue;
            
            float dist = Vector3Distance(playerPos, cards[i].base.base.position);
            
            if (dist <= cards[i].base.interactRange && dist < closestDistance) {
                closestDistance = dist;
                closestIndex = i;
            }
        }
        
        // Interact with closest card on E press
        if (closestIndex != -1 && IsKeyPressed(KEY_E)) {
            Interactable_Interact(&cards[closestIndex].base);
        }
        
        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);
            
            BeginMode3D(*Player_GetCamera(&player));
                // Draw ground plane
                DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){50.0f, 50.0f}, LIGHTGRAY);
                DrawGrid(50, 1.0f);
                
                // Draw cards
                Camera3D camera = *Player_GetCamera(&player);
                for (int i = 0; i < cardCount; i++) {
                    bool isClosest = (i == closestIndex);
                    Card_Draw(&cards[i], isClosest, camera);
                }
                
            EndMode3D();
            
            // Draw UI
            DrawText("WASD - Move | Mouse - Look | E - Interact", 10, 10, 20, BLACK);
            
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
    CloseWindow();

    return 0;
}
