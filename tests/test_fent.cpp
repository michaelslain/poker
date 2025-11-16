#include "catch_amalgamated.hpp"
#include "substances/fent.hpp"
#include "entities/player.hpp"
#include "core/physics.hpp"
#include "items/inventory.hpp"

TEST_CASE("Fent - Construction", "[substance][fent]") {
    SECTION("Create fent") {
        Fent fent({1, 2, 3}, nullptr);
        REQUIRE(fent.position.x == 1.0f);
        REQUIRE(fent.position.y == 2.0f);
        REQUIRE(fent.position.z == 3.0f);
    }
    
    SECTION("Fent has correct name") {
        Fent fent({0, 0, 0}, nullptr);
        REQUIRE(std::string(fent.GetName()) == "Fent");
    }
}

TEST_CASE("Fent - Type System", "[substance][fent]") {
    Fent fent({0, 0, 0}, nullptr);
    std::string type = fent.GetType();
    
    REQUIRE(type.find("object") != std::string::npos);
    REQUIRE(type.find("interactable") != std::string::npos);
    REQUIRE(type.find("item") != std::string::npos);
    REQUIRE(type.find("substance") != std::string::npos);
    REQUIRE(type.find("fent") != std::string::npos);
}

TEST_CASE("Fent - Clone", "[substance][fent]") {
    Fent original({1, 2, 3}, nullptr);
    Object* cloned = original.Clone({5, 6, 7});
    
    REQUIRE(cloned != nullptr);
    REQUIRE(cloned->position.x == 5.0f);
    REQUIRE(cloned->position.y == 6.0f);
    REQUIRE(cloned->position.z == 7.0f);
    REQUIRE(cloned->GetType().find("fent") != std::string::npos);
    
    delete cloned;
}

TEST_CASE("Fent - Death Trigger", "[substance][fent][death]") {
    PhysicsWorld physics;
    
    SECTION("Consume triggers death when player exists") {
        Player player({0, 0, 0}, &physics);
        Player::SetGlobal(&player);
        
        // Player should not be dying initially
        REQUIRE(player.IsDying() == false);
        REQUIRE(player.IsDead() == false);
        
        // Consume fent
        Fent fent({0, 0, 0}, nullptr);
        fent.Consume();
        
        // Player should now be dying
        REQUIRE(player.IsDying() == true);
        
        // Clear global player
        Player::SetGlobal(nullptr);
    }
    
    SECTION("Consume doesn't crash without player") {
        Player::SetGlobal(nullptr);
        
        Fent fent({0, 0, 0}, nullptr);
        // Should not crash
        fent.Consume();
        
        REQUIRE(true);  // If we got here, it didn't crash
    }
}

TEST_CASE("Fent - Stacking in Inventory", "[substance][fent][inventory]") {
    Inventory inventory;
    
    SECTION("Fent stacks correctly") {
        Fent* fent1 = new Fent({0, 0, 0}, nullptr);
        Fent* fent2 = new Fent({1, 1, 1}, nullptr);
        
        inventory.AddItem(fent1);
        inventory.AddItem(fent2);
        
        // Both fents should stack into one slot
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 2);
        
        // Both should point to the same item object (first one added)
        REQUIRE(inventory.GetStack(0)->item == fent1);
    }
    
    SECTION("Fent is stackable") {
        Fent fent({0, 0, 0}, nullptr);
        REQUIRE(fent.CanStack() == true);
    }
}

TEST_CASE("Fent - Usable Flag", "[substance][fent]") {
    Fent fent({0, 0, 0}, nullptr);
    
    // Substances should be usable
    REQUIRE(fent.usable == true);
}
