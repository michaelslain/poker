#include "enemy.hpp"

Enemy::Enemy(Vector3 pos, const std::string& enemyName)
    : Person(pos, enemyName) {
}

const char* Enemy::GetType() const {
    return "enemy";
}
