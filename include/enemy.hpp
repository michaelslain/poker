#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "raylib.h"
#include "person.hpp"

class Enemy : public Person {
public:
    Enemy(Vector3 pos, const std::string& enemyName = "Enemy");
    virtual ~Enemy() = default;

    // Override GetType for identification
    const char* GetType() const override;
};

#endif
