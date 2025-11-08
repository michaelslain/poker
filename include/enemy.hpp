#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "raylib.h"
#include "person.hpp"

class Enemy : public Person {
private:
    float thinkingTimer;    // Timer for AI delay
    float thinkingDuration; // How long to "think" before acting
    bool isThinking;        // Whether currently thinking about a bet
    int pendingAction;      // Cached betting decision

public:
    Enemy(Vector3 pos, const std::string& enemyName = "Enemy");
    virtual ~Enemy() = default;

    // Override GetType for identification
    std::string GetType() const override;
    
    // Override PromptBet for AI logic (random decision with delay)
    int PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount) override;
    
    // Override Update to handle thinking timer
    void Update(float deltaTime) override;
};

#endif
