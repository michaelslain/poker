#ifndef GAME_SCENE_HPP
#define GAME_SCENE_HPP

#include "../include/scene.hpp"
#include "../include/player.hpp"
#include "../include/physics.hpp"
#include "../include/dom.hpp"
#include "../include/poker_table.hpp"
#include "../include/wall.hpp"
#include "../include/light.hpp"
#include "../include/light_bulb.hpp"
#include "../include/spawner.hpp"
#include "../include/scene.hpp"
#include "../include/physics.hpp"

// Factory function to create the game scene
Scene* CreateGameScene(PhysicsWorld* physics);

#endif
