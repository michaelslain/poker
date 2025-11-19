#include "catch_amalgamated.hpp"
#include "entities/player.hpp"
#include "core/physics.hpp"
#include "core/dom.hpp"

TEST_CASE("Player - Construction", "[player]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Default construction") {
        Player player({0, 0, 0});
        REQUIRE(player.GetPosition().x == 0.0f);
        REQUIRE(player.GetPosition().y == 0.0f);
        REQUIRE(player.GetPosition().z == 0.0f);
    }
    
    SECTION("Construction with custom name") {
        Player player({1, 2, 3}, "TestPlayer");
        REQUIRE(player.GetName() == "TestPlayer");
    }
    
    SECTION("Initial state is not dying") {
        Player player({0, 0, 0});
        REQUIRE(player.IsDying() == false);
        REQUIRE(player.IsDead() == false);
    }
    
    SECTION("Initial insanity is zero") {
        Player player({0, 0, 0});
        REQUIRE(player.GetInsanity() == 0.0f);
    }
}

TEST_CASE("Player - Type System", "[player]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    Player player({0, 0, 0});
    
    std::string type = player.GetType();
    REQUIRE(type.find("object") != std::string::npos);
    REQUIRE(type.find("person") != std::string::npos);
    REQUIRE(type.find("player") != std::string::npos);
}

TEST_CASE("Player - Global Instance Management", "[player]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Global starts as null") {
        Player::SetGlobal(nullptr);
        REQUIRE(Player::GetGlobal() == nullptr);
    }
    
    SECTION("Can set and get global instance") {
        Player player({0, 0, 0});
        Player::SetGlobal(&player);
        REQUIRE(Player::GetGlobal() == &player);
        
        // Cleanup
        Player::SetGlobal(nullptr);
    }
    
    SECTION("Can change global instance") {
        Player player1({0, 0, 0});
        Player player2({1, 1, 1});
        
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
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("TriggerDeath sets dying state") {
        Player player({0, 0, 0});
        REQUIRE(player.IsDying() == false);
        
        player.TriggerDeath();
        REQUIRE(player.IsDying() == true);
        REQUIRE(player.IsDead() == false); // Not dead yet, vignette just started
    }
    
    SECTION("TriggerDeath can only be called once") {
        Player player({0, 0, 0});
        player.TriggerDeath();
        player.TriggerDeath(); // Should be idempotent
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death progresses over time") {
        Player player({0, 0, 0});
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
        Player player({0, 0, 0});
        player.TriggerDeath();
        
        // Update for exactly 3 seconds
        player.Update(3.0f);
        REQUIRE(player.IsDead() == true);
    }
    
    SECTION("Death vignette clamps at 100%") {
        Player player({0, 0, 0});
        player.TriggerDeath();
        
        // Update way past 3 seconds
        player.Update(10.0f);
        REQUIRE(player.IsDead() == true);
        // Should still be dead, not overflow
    }
    
    SECTION("While dying, player stops updating") {
        Player player({0, 0, 0});
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
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("100% insanity triggers death") {
        Player player({0, 0, 0});
        
        // Stand still for long enough to reach 100% insanity
        // Rate is 0.02/sec standing, so need 50 seconds
        player.Update(50.0f);
        
        REQUIRE(player.GetInsanity() >= 1.0f);
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death doesn't trigger until insanity reaches 100%") {
        Player player({0, 0, 0});
        
        // Stand still for 40 seconds (80% insanity)
        player.Update(40.0f);
        
        REQUIRE(player.GetInsanity() < 1.0f);
        REQUIRE(player.IsDying() == false);
    }
}

TEST_CASE("Player - Insanity Integration", "[player][insanity]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Player has insanity manager") {
        Player player({0, 0, 0});
        REQUIRE(player.GetInsanity() == 0.0f);
    }
    
    SECTION("OnKillPerson increases minimum insanity") {
        Player player({0, 0, 0});
        player.OnKillPerson();
        
        // After one kill, should have min insanity of 0.2
        REQUIRE(player.GetInsanity() >= 0.2f);
    }
    
    SECTION("Multiple kills stack insanity") {
        Player player({0, 0, 0});
        player.OnKillPerson();
        player.OnKillPerson();
        player.OnKillPerson();
        
        // 3 kills = 0.6 min insanity
        REQUIRE(player.GetInsanity() >= 0.6f);
    }
}

TEST_CASE("Player - Inventory Access", "[player][inventory]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Player has inventory") {
        Player player({0, 0, 0});
        REQUIRE(player.GetInventory() != nullptr);
    }
    
    SECTION("Inventory starts empty") {
        Player player({0, 0, 0});
        REQUIRE(player.GetInventory()->GetStackCount() == 0);
    }
}

TEST_CASE("Player - Camera Access", "[player][camera]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Player has camera") {
        Player player({0, 0, 0});
        REQUIRE(player.GetCamera() != nullptr);
    }
    
    SECTION("Camera position follows player") {
        Player player({5, 10, 15});
        Camera3D* cam = player.GetCamera();
        
        // Camera should be at player position + eye height
        REQUIRE(cam->position.x == 5.0f);
        REQUIRE(cam->position.z == 15.0f);
        // Y is offset by eye height
    }
}

TEST_CASE("Player - Seating System", "[player][seating]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Player starts not seated") {
        Player player({0, 0, 0});
        REQUIRE(player.IsSeated() == false);
    }
    
    SECTION("Can sit down at position") {
        Player player({0, 0, 0});
        player.SitDown({5, 1, 5});
        
        REQUIRE(player.IsSeated() == true);
    }
    
    SECTION("Can stand up") {
        Player player({0, 0, 0});
        player.SitDown({5, 1, 5});
        player.StandUp();
        
        REQUIRE(player.IsSeated() == false);
    }
}

TEST_CASE("Player - Physics Geometry", "[player][physics]") {
    PhysicsWorld physics;
    PhysicsWorld::SetGlobal(&physics);
    
    SECTION("Player has physics geometry") {
        Player player({0, 0, 0});
        REQUIRE(player.GetGeom() != nullptr);
    }
    
    PhysicsWorld::SetGlobal(nullptr);  // Clean up
}

TEST_CASE("Player - Selected Item Index", "[player][inventory]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Selected item starts at -1 (none)") {
        Player player({0, 0, 0});
        REQUIRE(player.GetSelectedItemIndex() == -1);
    }
}

TEST_CASE("Player - Death System Edge Cases", "[player][death][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);  // Set global physics for Player constructor
    DOM::SetGlobal(&dom);  // Set global DOM
    
    SECTION("Can trigger death multiple times safely") {
        Player player({0, 0, 0});
        player.TriggerDeath();
        player.TriggerDeath();
        player.TriggerDeath();
        
        REQUIRE(player.IsDying() == true);
    }
    
    SECTION("Death state persists across updates") {
        Player player({0, 0, 0});
        player.TriggerDeath();
        
        player.Update(1.0f);
        REQUIRE(player.IsDying() == true);
        
        player.Update(1.0f);
        REQUIRE(player.IsDying() == true);
        
        player.Update(1.0f);
        REQUIRE(player.IsDead() == true);
    }
    
    SECTION("Zero delta time while dying") {
        Player player({0, 0, 0});
        player.TriggerDeath();
        
        player.Update(0.0f);
        REQUIRE(player.IsDying() == true);
        REQUIRE(player.IsDead() == false);
    }
}

// Helper class to access protected members for testing
class TestablePlayer : public Player {
public:
    TestablePlayer(Vector3 pos) : Player(pos) {}
    dBodyID GetBody() const { return body; }
};

TEST_CASE("Player - Coordinate System", "[player][physics][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Player spawns with correct drawing reference height") {
        Player player({0, 1.3f, 0});
        
        REQUIRE(player.position.y == Catch::Approx(1.3f));
    }
    
    SECTION("Player physics body positioned correctly") {
        TestablePlayer player({0, 0.0f, 0});  // NEW SYSTEM: feet at Y=0
        
        // For normal height player, body center should be at 1.7
        if (player.GetBody() != nullptr) {
            const dReal* bodyPos = dBodyGetPosition(player.GetBody());
            REQUIRE(bodyPos[1] == Catch::Approx(1.7f).margin(0.01));
        }
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("Player - Teleport", "[player][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Teleport updates position and physics") {
        TestablePlayer player({0, 0.0f, 0});  // NEW SYSTEM: feet at Y=0
        
        player.Teleport({10.0f, 0.0f, 5.0f});  // NEW SYSTEM: feet at Y=0
        
        REQUIRE(player.position.x == Catch::Approx(10.0f).margin(0.1f));
        REQUIRE(player.position.y == Catch::Approx(0.0f).margin(0.1f));
        REQUIRE(player.position.z == Catch::Approx(5.0f).margin(0.1f));
        
        // Physics body should also be updated
        if (player.GetBody() != nullptr) {
            const dReal* bodyPos = dBodyGetPosition(player.GetBody());
            REQUIRE(bodyPos[0] == Catch::Approx(10.0f));
            // Body Y should be at center height (1.7)
            REQUIRE(bodyPos[1] == Catch::Approx(1.7f).margin(0.01));
            REQUIRE(bodyPos[2] == Catch::Approx(5.0f));
        }
    }
    
    SECTION("Teleport resets velocity") {
        TestablePlayer player({0, 1.3f, 0});
        
        // Give player velocity
        if (player.GetBody() != nullptr) {
            dBodySetLinearVel(player.GetBody(), 5, -10, 3);
        }
        
        player.Teleport({10.0f, 1.3f, 5.0f});
        
        // Velocity should be reset to zero
        if (player.GetBody() != nullptr) {
            const dReal* vel = dBodyGetLinearVel(player.GetBody());
            REQUIRE(vel[0] == Catch::Approx(0.0f));
            REQUIRE(vel[1] == Catch::Approx(0.0f));
            REQUIRE(vel[2] == Catch::Approx(0.0f));
        }
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}

TEST_CASE("Player - Seating Y Preservation", "[player][seating][regression]") {
    PhysicsWorld physics;
    DOM dom;
    PhysicsWorld::SetGlobal(&physics);
    DOM::SetGlobal(&dom);
    
    SECTION("Sitting preserves Y position") {
        Player player({0, 1.3f, 0});
        float originalY = player.position.y;
        
        player.SitDown({5.0f, originalY, 3.0f});
        
        REQUIRE(player.IsSeated() == true);
        REQUIRE(player.position.y == Catch::Approx(originalY));
    }
    
    SECTION("Standing preserves Y position") {
        Player player({0, 1.3f, 0});
        
        player.SitDown({5.0f, 1.3f, 3.0f});
        float seatedY = player.position.y;
        
        player.StandUp();
        
        REQUIRE(player.IsSeated() == false);
        REQUIRE(player.position.y == Catch::Approx(seatedY));
    }
    
    // Cleanup
    PhysicsWorld::SetGlobal(nullptr);
    DOM::SetGlobal(nullptr);
}
