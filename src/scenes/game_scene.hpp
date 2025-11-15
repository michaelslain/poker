#ifndef GAME_SCENE_HPP
#define GAME_SCENE_HPP

#include "core/scene.hpp"
#include "core/physics.hpp"

// Factory function to create the game scene
Scene* CreateGameScene(PhysicsWorld* physics);

#endif
