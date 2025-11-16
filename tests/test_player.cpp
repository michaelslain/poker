#include "catch_amalgamated.hpp"
#include "entities/player.hpp"
#include "core/physics.hpp"
#include "core/dom.hpp"

TEST_CASE("Player - Construction", "[player]") {
    PhysicsWorld physics;
    
    SECTION("Default construction") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetPosition().x == 0.0f);
        REQUIRE(player.GetPosition().y == 0.0f);
        REQUIRE(player.GetPosition().z == 0.0f);
    }
    
    SECTION("Construction with custom name") {
        Player player({1, 2, 3}, &physics, "TestPlayer");
        REQUIRE(player.GetName() == "TestPlayer");
    }
    
    SECTION("Initial state is not dying") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.IsDying() == false);
        REQUIRE(player.IsDead() == false);
    }
    
    SECTION("Initial insanity is zero") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetInsanity() == 0.0f);
    }
}

TEST_CASE("Player - Type System", "[player]") {
    PhysicsWorld physics;
    Player player({0, 0, 0}, &physics);
    
    std::string type = player.GetType();
    REQUIRE(type.find("object") != std::string::npos);
    REQUIRE(type.find("person") != std::string::npos);
    REQUIRE(type.find("player") != std::string::npos);
}

TEST_CASE("Player - Global Instance Management", "[player]") {
    PhysicsWorld physics;
    
    SECTION("Global starts as null") {
        Player::SetGlobal(nullptr);
        REQUIRE(Player::GetGlobal() == nullptr);
    }
    
    SECTION("Can set and get global instance") {
        Player player({0, 0, 0}, &physics);
        Player::SetGlobal(&player);
        REQUIRE(Player::GetGlobal() == &player);
        
        // Cleanup
        Player::SetGlobal(nullptr);
    }
    
    SECTION("Can change global instance") {
        Player player1({0, 0, 0}, &physics);
        Player player2({1, 1, 1}, &physics);
        
        Player::SetGlobal(&player1);
        REQUIRE(Player::GetGlobal() == &player1);
        
        Player::SetGlobal(&player2);
        REQUIRE(Player::GetGlobal() == &player2);
        
        // Cleanup
        Player::SetGlobal(nullptr);
    }
}

TEST_CASE("Player - Death System", "[player][death]") {
    PhysicsWorld physics;
    
    SECTION("TriggerDeath sets dying state") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.IsDying() == false);
        
        player.TriggerDeath();
        REQUIRE(player.IsDying() == true);
        REQUIRE(player.IsDead() == false); // Not dead yet, vignette just started
    }
    
    SECTION("TriggerDeath can only be called once") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        player.TriggerDeath(); // Should be idempotent
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death progresses over time") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        
        // Update for 1 second (33% of 3-second duration)
        player.Update(1.0f);
        REQUIRE(player.IsDying() == true);
        REQUIRE(player.IsDead() == false); // Still not complete
        
        // Update for 2 more seconds (total 3 seconds)
        player.Update(2.0f);
        REQUIRE(player.IsDead() == true); // Now complete
    }
    
    SECTION("Death completes after 3 seconds") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        
        // Update for exactly 3 seconds
        player.Update(3.0f);
        REQUIRE(player.IsDead() == true);
    }
    
    SECTION("Death vignette clamps at 100%") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        
        // Update way past 3 seconds
        player.Update(10.0f);
        REQUIRE(player.IsDead() == true);
        // Should still be dead, not overflow
    }
    
    SECTION("While dying, player stops updating") {
        Player player({0, 0, 0}, &physics);
        Vector3 initialPos = player.GetPosition();
        
        player.TriggerDeath();
        
        // Try to update for 0.5 seconds
        player.Update(0.5f);
        
        // Position should not change (update returns early when dying)
        REQUIRE(player.GetPosition().x == initialPos.x);
        REQUIRE(player.GetPosition().y == initialPos.y);
        REQUIRE(player.GetPosition().z == initialPos.z);
    }
}

TEST_CASE("Player - Death from Insanity", "[player][death][insanity]") {
    PhysicsWorld physics;
    
    SECTION("100% insanity triggers death") {
        Player player({0, 0, 0}, &physics);
        
        // Stand still for long enough to reach 100% insanity
        // Rate is 0.02/sec standing, so need 50 seconds
        player.Update(50.0f);
        
        REQUIRE(player.GetInsanity() >= 1.0f);
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death doesn't trigger until insanity reaches 100%") {
        Player player({0, 0, 0}, &physics);
        
        // Stand still for 40 seconds (80% insanity)
        player.Update(40.0f);
        
        REQUIRE(player.GetInsanity() < 1.0f);
        REQUIRE(player.IsDying() == false);
    }
}

TEST_CASE("Player - Insanity Integration", "[player][insanity]") {
    PhysicsWorld physics;
    
    SECTION("Player has insanity manager") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetInsanity() == 0.0f);
    }
    
    SECTION("OnKillPerson increases minimum insanity") {
        Player player({0, 0, 0}, &physics);
        player.OnKillPerson();
        
        // After one kill, should have min insanity of 0.2
        REQUIRE(player.GetInsanity() >= 0.2f);
    }
    
    SECTION("Multiple kills stack insanity") {
        Player player({0, 0, 0}, &physics);
        player.OnKillPerson();
        player.OnKillPerson();
        player.OnKillPerson();
        
        // 3 kills = 0.6 min insanity
        REQUIRE(player.GetInsanity() >= 0.6f);
    }
}

TEST_CASE("Player - Inventory Access", "[player][inventory]") {
    PhysicsWorld physics;
    
    SECTION("Player has inventory") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetInventory() != nullptr);
    }
    
    SECTION("Inventory starts empty") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetInventory()->GetStackCount() == 0);
    }
}

TEST_CASE("Player - Camera Access", "[player][camera]") {
    PhysicsWorld physics;
    
    SECTION("Player has camera") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetCamera() != nullptr);
    }
    
    SECTION("Camera position follows player") {
        Player player({5, 10, 15}, &physics);
        Camera3D* cam = player.GetCamera();
        
        // Camera should be at player position + eye height
        REQUIRE(cam->position.x == 5.0f);
        REQUIRE(cam->position.z == 15.0f);
        // Y is offset by eye height
    }
}

TEST_CASE("Player - Seating System", "[player][seating]") {
    PhysicsWorld physics;
    
    SECTION("Player starts not seated") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.IsSeated() == false);
    }
    
    SECTION("Can sit down at position") {
        Player player({0, 0, 0}, &physics);
        player.SitDown({5, 1, 5});
        
        REQUIRE(player.IsSeated() == true);
    }
    
    SECTION("Can stand up") {
        Player player({0, 0, 0}, &physics);
        player.SitDown({5, 1, 5});
        player.StandUp();
        
        REQUIRE(player.IsSeated() == false);
    }
}

TEST_CASE("Player - Physics Geometry", "[player][physics]") {
    PhysicsWorld physics;
    
    SECTION("Player has physics geometry") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetGeom() != nullptr);
    }
}

TEST_CASE("Player - Selected Item Index", "[player][inventory]") {
    PhysicsWorld physics;
    
    SECTION("Selected item starts at -1 (none)") {
        Player player({0, 0, 0}, &physics);
        REQUIRE(player.GetSelectedItemIndex() == -1);
    }
}

TEST_CASE("Player - Death System Edge Cases", "[player][death][regression]") {
    PhysicsWorld physics;
    
    SECTION("Can trigger death multiple times safely") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        player.TriggerDeath();
        player.TriggerDeath();
        
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death state persists across updates") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        
        player.Update(1.0f);
        REQUIRE(player.IsDying() == true);
        
        player.Update(1.0f);
        REQUIRE(player.IsDying() == true);
        
        player.Update(1.0f);
        REQUIRE(player.IsDead() == true);
    }
    
    SECTION("Zero delta time while dying") {
        Player player({0, 0, 0}, &physics);
        player.TriggerDeath();
        
        player.Update(0.0f);
        REQUIRE(player.IsDying() == true);
        REQUIRE(player.IsDead() == false);
    }
}
