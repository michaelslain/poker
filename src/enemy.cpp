#include "enemy.hpp"

Enemy::Enemy(Vector3 pos, const std::string& enemyName)
    : Person(pos, enemyName, 1.5f) {  // Enemies are 1.5x taller than normal
}

const char* Enemy::GetType() const {
    return "enemy";
}
