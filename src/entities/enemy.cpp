#include "entities/enemy.hpp"
#include "rendering/psychedelic_manager.hpp"
#include "items/card.hpp"
#include "rlgl.h"
#include "raymath.h"
#include <cstdlib>
#include <cmath>

Enemy::Enemy(Vector3 pos, const std::string& enemyName)
    : Person(pos, enemyName, 1.5f),  // Enemies are 1.5x taller than normal
      thinkingTimer(0.0f),
      thinkingDuration(0.0f),
      isThinking(false),
      pendingAction(-1) {
    // Enemies use default Person rendering (pitch black, usesLighting = false)
}

std::string Enemy::GetType() const {
    return Person::GetType() + "_enemy";
}

void Enemy::Update(float deltaTime) {
    // Call parent update
    Person::Update(deltaTime);
    
    // Update thinking timer if currently thinking
    if (isThinking) {
        thinkingTimer += deltaTime;
    }
}

int Enemy::PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount) {
    (void)currentBet;  // Suppress unused warning
    
    // First call: Start thinking
    if (!isThinking) {
        isThinking = true;
        thinkingTimer = 0.0f;
        // Random thinking time between 2 and 4 seconds
        thinkingDuration = 2.0f + ((rand() % 200) / 100.0f);
        pendingAction = -1;
        return -1;  // Still thinking
    }
    
    // Still thinking...
    if (thinkingTimer < thinkingDuration) {
        return -1;  // Still thinking
    }
    
    // Done thinking - make decision (only once)
    if (pendingAction == -1) {
        // AI logic: Random decision between fold, call, raise
        int decision = rand() % 3;  // 0=fold, 1=call, 2=raise
        
        // Count total chip value in inventory
        Inventory* inv = GetInventory();
        if (!inv) {
            pendingAction = 0;  // Fold if no inventory
            isThinking = false;
            return 0;
        }
        
        int totalChipValue = inv->GetTotalChipValue();
        
        // If we can't afford to call, must fold
        if (callAmount > totalChipValue && decision != 0) {
            pendingAction = 0;  // Fold
        }
        else if (decision == 2) {  // Raise
            // Can we afford minimum raise?
            if (minRaise > maxRaise) {
                pendingAction = 1;  // Call instead
            } else {
                // Choose random raise amount between minRaise and maxRaise
                int raiseRange = maxRaise - minRaise;
                raiseAmount = minRaise + (rand() % (raiseRange + 1));
                pendingAction = 2;  // Raise
            }
        } else if (decision == 1) {
            pendingAction = 1;  // Call
        } else {
            pendingAction = 0;  // Fold
        }
    }
    
    // Reset for next time and return decision
    isThinking = false;
    thinkingTimer = 0.0f;
    int action = pendingAction;
    pendingAction = -1;
    return action;
}

void Enemy::Draw(Camera3D camera) {
    // Call parent draw (renders the person body)
    Person::Draw(camera);
    
    // Draw holographic cards if on molly
    DrawHolographicCards(camera);
}

void Enemy::DrawHolographicCards(Camera3D camera) {
    // Only render cards when player is on molly
    if (!PsychedelicManager::IsTripping() || PsychedelicManager::GetTripType() != TripType::MOLLY) {
        return;
    }
    
    // Search inventory for cards using the helper method
    Inventory* inv = GetInventory();
    if (!inv) return;
    
    std::vector<int> cardIndices = inv->GetIndicesByType("card");
    if (cardIndices.empty()) return;
    
    // Collect card pointers
    std::vector<Card*> cards;
    for (int idx : cardIndices) {
        ItemStack* stack = inv->GetStack(idx);
        if (stack && stack->item) {
            cards.push_back(static_cast<Card*>(stack->item));
        }
    }
    
    if (cards.empty()) return;
    
    // Calculate position above enemy's head
    // Person::Draw renders head at: meshOffset + 2.1*height, where meshOffset = 1.3*height
    // So head top is at: position.y + (1.3 + 2.1 + 0.3) * height = position.y + 3.7 * height
    // (0.3 accounts for head sphere radius)
    float headTopHeight = position.y + (3.7f * height) + 0.3f;  // Extra 0.3 for floating above
    Vector3 aboveHead = {position.x, headTopHeight, position.z};
    
    // Render each card as a holographic floating card
    float cardSpacing = 0.3f;  // Space between cards
    float totalWidth = (cards.size() - 1) * cardSpacing;
    float startX = -totalWidth / 2.0f;  // Center the cards
    
    for (size_t i = 0; i < cards.size(); i++) {
        Card* card = cards[i];
        
        // Calculate card position (spread horizontally)
        Vector3 cardPos = aboveHead;
        cardPos.x += startX + (i * cardSpacing);
        
        // Add gentle floating animation
        float time = GetTime();
        cardPos.y += sinf(time * 2.0f + i * 0.5f) * 0.05f;  // Gentle bob
        
        // Calculate rotation to face camera
        Vector3 camPos = camera.position;
        Vector3 toCamera = {camPos.x - cardPos.x, 0, camPos.z - cardPos.z};
        float angle = atan2f(toCamera.x, toCamera.z);
        
        // Scale for the holographic card
        float cardScale = 0.5f;
        
        // Card dimensions
        float cardWidth = 0.5f;
        float cardHeight = 0.7f;
        
        // Draw the card with pink holographic effect
        rlPushMatrix();
            rlTranslatef(cardPos.x, cardPos.y, cardPos.z);
            rlRotatef(angle * RAD2DEG, 0, 1, 0);  // Face camera
            rlScalef(cardScale, cardScale, cardScale);
            
            // Draw card with texture and pink tint
            if (card->textureLoaded) {
                Color pinkTint = {255, 180, 220, 200};  // Semi-transparent pink
                
                rlDisableBackfaceCulling();
                rlSetTexture(card->texture.texture.id);
                rlBegin(RL_QUADS);
                    rlColor4ub(pinkTint.r, pinkTint.g, pinkTint.b, pinkTint.a);
                    rlNormal3f(0.0f, 0.0f, 1.0f);
                    rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-cardWidth/2, -cardHeight/2, 0.0f);
                    rlTexCoord2f(1.0f, 0.0f); rlVertex3f(cardWidth/2, -cardHeight/2, 0.0f);
                    rlTexCoord2f(1.0f, 1.0f); rlVertex3f(cardWidth/2, cardHeight/2, 0.0f);
                    rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-cardWidth/2, cardHeight/2, 0.0f);
                rlEnd();
                rlSetTexture(0);
                rlEnableBackfaceCulling();
            }
            
        rlPopMatrix();
    }
}
