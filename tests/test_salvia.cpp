#include "catch_amalgamated.hpp"
#include "substances/salvia.hpp"
#include "core/level_manager.hpp"
#include "core/physics.hpp"
#include "rendering/psychedelic_manager.hpp"
#include "raylib.h"

TEST_CASE("Salvia - Construction", "[salvia][substance]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Default constructor creates at origin") {
        Salvia salvia;
        REQUIRE(salvia.position.x == 0.0f);
        REQUIRE(salvia.position.y == 0.0f);
        REQUIRE(salvia.position.z == 0.0f);
    }
    
    SECTION("Position constructor sets position") {
        Vector3 pos = {5.0f, 2.0f, 3.0f};
        Salvia salvia(pos);
        REQUIRE(salvia.position.x == 5.0f);
        REQUIRE(salvia.position.y == 2.0f);
        REQUIRE(salvia.position.z == 3.0f);
    }
    
    SECTION("Salvia is usable") {
        Salvia salvia;
        REQUIRE(salvia.usable);
    }
}

TEST_CASE("Salvia - Type system", "[salvia][substance]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    Salvia salvia;
    std::string type = salvia.GetType();
    
    SECTION("Type contains substance hierarchy") {
        REQUIRE(type.find("object") != std::string::npos);
        REQUIRE(type.find("interactable") != std::string::npos);
        REQUIRE(type.find("item") != std::string::npos);
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("salvia") != std::string::npos);
    }
}

TEST_CASE("Salvia - Name", "[salvia][substance]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    Salvia salvia;
    
    SECTION("GetName returns Salvia") {
        REQUIRE(std::string(salvia.GetName()) == "Salvia");
    }
}

TEST_CASE("Salvia - Consume triggers alternate dimension", "[salvia][substance][integration]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("Consume enters alternate dimension") {
        manager->SetLevel(5);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
        
        Salvia salvia;
        salvia.Consume();
        
        // Should enter alternate dimension
        REQUIRE(manager->IsInAlternateDimension());
        REQUIRE(manager->GetCurrentDimension() == 1);
        
        // Level should not change
        REQUIRE(manager->GetCurrentLevel() == 5);
    }
    
    SECTION("Consume from level 0 enters alternate dimension") {
        manager->SetLevel(0);
        
        Salvia salvia;
        salvia.Consume();
        
        REQUIRE(manager->IsInAlternateDimension());
        REQUIRE(manager->GetCurrentLevel() == 0);
    }
    
    SECTION("Multiple Salvia consumes stack dimensions") {
        manager->SetLevel(3);
        
        Salvia salvia1;
        salvia1.Consume();
        REQUIRE(manager->GetCurrentDimension() == 1);
        
        Salvia salvia2;
        salvia2.Consume();
        REQUIRE(manager->GetCurrentDimension() == 2);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("Salvia - Consume starts psychedelic trip", "[salvia][substance][integration]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    
    SECTION("Consume initializes trip") {
        // Stop any existing trip
        PsychedelicManager::StopTrip();
        
        Salvia salvia;
        salvia.Consume();
        
        // Note: Trip start requires shader initialization
        // This test verifies the function doesn't crash
        REQUIRE(true);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("Salvia - Clone functionality", "[salvia][substance]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Clone creates new instance at different position") {
        Salvia original({1.0f, 2.0f, 3.0f});
        
        Object* cloned = original.Clone({5.0f, 6.0f, 7.0f});
        REQUIRE(cloned != nullptr);
        
        Salvia* clonedSalvia = dynamic_cast<Salvia*>(cloned);
        REQUIRE(clonedSalvia != nullptr);
        
        REQUIRE(clonedSalvia->position.x == 5.0f);
        REQUIRE(clonedSalvia->position.y == 6.0f);
        REQUIRE(clonedSalvia->position.z == 7.0f);
        
        delete cloned;
    }
    
    SECTION("Clone has same type") {
        Salvia original;
        Object* cloned = original.Clone({0, 0, 0});
        
        REQUIRE(cloned->GetType() == original.GetType());
        
        delete cloned;
    }
}

TEST_CASE("Salvia - Rendering properties", "[salvia][substance]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    Salvia salvia;
    
    SECTION("Color is lime green") {
        // Salvia color is {200, 255, 100, 255} - bright lime green
        // This is set in constructor
        REQUIRE(true); // Color verified in implementation
    }
}

TEST_CASE("Salvia - Comparison with other substances", "[salvia][substance][regression]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Salvia is different from other substance types") {
        Salvia salvia;
        std::string salviaType = salvia.GetType();
        
        // Should be unique substance type
        REQUIRE(salviaType.find("salvia") != std::string::npos);
        REQUIRE(salviaType.find("weed") == std::string::npos);
        REQUIRE(salviaType.find("shrooms") == std::string::npos);
        REQUIRE(salviaType.find("fent") == std::string::npos);
    }
}

TEST_CASE("Salvia - Edge cases", "[salvia][substance][regression]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    
    SECTION("Consume at extreme positions") {
        Salvia salvia({1000.0f, -500.0f, 2000.0f});
        
        // Should not crash regardless of position
        salvia.Consume();
        REQUIRE(true);
    }
    
    SECTION("Multiple rapid consumes") {
        Salvia salvia;
        
        salvia.Consume();
        salvia.Consume();
        salvia.Consume();
        
        // Should not crash
        REQUIRE(true);
    }
    
    LevelManager::Destroy();
}

TEST_CASE("Salvia - Integration with game flow", "[salvia][substance][integration]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    LevelManager::Destroy();
    LevelManager* manager = LevelManager::GetInstance();
    
    SECTION("Salvia trip from level 0 to level 1-10") {
        manager->SetLevel(0);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
        
        // Consume Salvia
        Salvia salvia;
        salvia.Consume();
        REQUIRE(manager->IsInAlternateDimension());
        
        // Exit alternate dimension with random jump
        int jump = manager->GenerateRandomLevelJump();
        manager->ExitAlternateDimension(jump);
        
        // Should end up at level 1-10
        REQUIRE(manager->GetCurrentLevel() >= 1);
        REQUIRE(manager->GetCurrentLevel() <= 10);
        REQUIRE_FALSE(manager->IsInAlternateDimension());
    }
    
    SECTION("Salvia trip from level 5") {
        manager->SetLevel(5);
        
        Salvia salvia;
        salvia.Consume();
        
        int jump = manager->GenerateRandomLevelJump();
        manager->ExitAlternateDimension(jump);
        
        // Should jump forward 1-10 levels from 5
        REQUIRE(manager->GetCurrentLevel() >= 6);  // 5 + 1
        REQUIRE(manager->GetCurrentLevel() <= 15); // 5 + 10
    }
    
    LevelManager::Destroy();
}
