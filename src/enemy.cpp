#include "enemy.hpp"
#include "chip.hpp"
#include <cstdlib>

Enemy::Enemy(Vector3 pos, const std::string& enemyName)
    : Person(pos, enemyName, 1.5f),  // Enemies are 1.5x taller than normal
      thinkingTimer(0.0f),
      thinkingDuration(0.0f),
      isThinking(false),
      pendingAction(-1) {
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
    
    printf("[ENEMY %s PromptBet] this=%p, isThinking=%d, timer=%.2f/%.2f\n",
           name.c_str(), (void*)this, isThinking, thinkingTimer, thinkingDuration);
    
    // First call: Start thinking
    if (!isThinking) {
        isThinking = true;
        thinkingTimer = 0.0f;
        // Random thinking time between 2 and 4 seconds
        thinkingDuration = 2.0f + ((rand() % 200) / 100.0f);
        pendingAction = -1;
        
        printf("[ENEMY %s] STARTED THINKING, duration set to %.2f\n", name.c_str(), thinkingDuration);        return -1;  // Still thinking
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
