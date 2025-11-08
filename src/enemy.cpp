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
    return "enemy";
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
        
        TraceLog(LOG_INFO, "Enemy %s: Started thinking (%.1f seconds)...", name.c_str(), thinkingDuration);
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
            TraceLog(LOG_INFO, "Enemy %s: ERROR - Null inventory, folding", name.c_str());
            pendingAction = 0;  // Fold if no inventory
            isThinking = false;
            return 0;
        }
        
        int totalChipValue = 0;
        for (int i = 0; i < inv->GetStackCount(); i++) {
            ItemStack* stack = inv->GetStack(i);
            if (stack && stack->item) {
                std::string type = stack->item->GetType();
                if (type.substr(0, 5) == "chip_") {
                    Chip* chip = static_cast<Chip*>(stack->item);
                    totalChipValue += chip->value * stack->count;
                }
            }
        }
        
        // If we can't afford to call, must fold
        if (callAmount > totalChipValue && decision != 0) {
            TraceLog(LOG_INFO, "Enemy %s: Can't afford call (%d), only have %d chips, folding", 
                     name.c_str(), callAmount, totalChipValue);
            pendingAction = 0;  // Fold
        }
        else if (decision == 2) {  // Raise
            // Can we afford minimum raise?
            if (minRaise > maxRaise) {
                TraceLog(LOG_INFO, "Enemy %s: Can't afford raise, calling instead", name.c_str());
                pendingAction = 1;  // Call instead
            } else {
                // Choose random raise amount between minRaise and maxRaise
                int raiseRange = maxRaise - minRaise;
                raiseAmount = minRaise + (rand() % (raiseRange + 1));
                
                TraceLog(LOG_INFO, "Enemy %s: Raises to %d", name.c_str(), raiseAmount);
                pendingAction = 2;  // Raise
            }
        } else if (decision == 1) {
            TraceLog(LOG_INFO, "Enemy %s: Calls %d", name.c_str(), callAmount);
            pendingAction = 1;  // Call
        } else {
            TraceLog(LOG_INFO, "Enemy %s: Folds", name.c_str());
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
