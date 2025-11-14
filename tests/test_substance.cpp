#include "catch_amalgamated.hpp"
#include "../include/substance.hpp"
#include "../include/adrenaline.hpp"
#include "../include/salvia.hpp"
#include "../include/cocaine.hpp"
#include "../include/shrooms.hpp"
#include "../include/vodka.hpp"
#include "../include/weed.hpp"
#include "../include/molly.hpp"
#include "../include/inventory.hpp"

TEST_CASE("Adrenaline - Construction", "[substance][adrenaline]") {
    SECTION("Create adrenaline") {
        Adrenaline adrenaline({1, 2, 3}, nullptr);
        REQUIRE(adrenaline.position.x == 1.0f);
        REQUIRE(adrenaline.position.y == 2.0f);
        REQUIRE(adrenaline.position.z == 3.0f);
    }
    
    SECTION("Adrenaline has correct name") {
        Adrenaline adrenaline({0, 0, 0}, nullptr);
        REQUIRE(std::string(adrenaline.GetName()) == "Adrenaline");
    }
}

TEST_CASE("Adrenaline - Type System", "[substance][adrenaline]") {
    Adrenaline adrenaline({0, 0, 0}, nullptr);
    std::string type = adrenaline.GetType();
    
    REQUIRE(type.find("object") != std::string::npos);
    REQUIRE(type.find("interactable") != std::string::npos);
    REQUIRE(type.find("item") != std::string::npos);
    REQUIRE(type.find("substance") != std::string::npos);
    REQUIRE(type.find("adrenaline") != std::string::npos);
}

TEST_CASE("Salvia - Construction and Type", "[substance][salvia]") {
    Salvia salvia({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(salvia.GetName()) == "Salvia");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = salvia.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("salvia") != std::string::npos);
    }
}

TEST_CASE("Cocaine - Construction and Type", "[substance][cocaine]") {
    Cocaine cocaine({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(cocaine.GetName()) == "Cocaine");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = cocaine.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("cocaine") != std::string::npos);
    }
}

TEST_CASE("Shrooms - Construction and Type", "[substance][shrooms]") {
    Shrooms shrooms({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(shrooms.GetName()) == "Shrooms");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = shrooms.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("shrooms") != std::string::npos);
    }
}

TEST_CASE("Vodka - Construction and Type", "[substance][vodka]") {
    Vodka vodka({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(vodka.GetName()) == "Vodka");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = vodka.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("vodka") != std::string::npos);
    }
}

TEST_CASE("Weed - Construction and Type", "[substance][weed]") {
    Weed weed({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(weed.GetName()) == "Weed");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = weed.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("weed") != std::string::npos);
    }
}

TEST_CASE("Molly - Construction and Type", "[substance][molly]") {
    Molly molly({0, 0, 0}, nullptr);
    
    SECTION("Has correct name") {
        REQUIRE(std::string(molly.GetName()) == "Molly");
    }
    
    SECTION("Has correct type hierarchy") {
        std::string type = molly.GetType();
        REQUIRE(type.find("substance") != std::string::npos);
        REQUIRE(type.find("molly") != std::string::npos);
    }
}

TEST_CASE("Substance - Clone", "[substance]") {
    SECTION("Adrenaline clones correctly") {
        Adrenaline original({1, 2, 3}, nullptr);
        Object* cloned = original.Clone({5, 6, 7});
        
        REQUIRE(cloned != nullptr);
        REQUIRE(cloned->position.x == 5.0f);
        REQUIRE(cloned->position.y == 6.0f);
        REQUIRE(cloned->position.z == 7.0f);
        REQUIRE(cloned->GetType().find("adrenaline") != std::string::npos);
        
        delete cloned;
    }
    
    SECTION("Cocaine clones correctly") {
        Cocaine original({0, 0, 0}, nullptr);
        Object* cloned = original.Clone({10, 11, 12});
        
        REQUIRE(cloned != nullptr);
        REQUIRE(cloned->position.x == 10.0f);
        REQUIRE(cloned->GetType().find("cocaine") != std::string::npos);
        
        delete cloned;
    }
}

TEST_CASE("Substance - Consume", "[substance]") {
    SECTION("All substances have Consume method") {
        Adrenaline adrenaline({0, 0, 0}, nullptr);
        Salvia salvia({0, 0, 0}, nullptr);
        Cocaine cocaine({0, 0, 0}, nullptr);
        Shrooms shrooms({0, 0, 0}, nullptr);
        Vodka vodka({0, 0, 0}, nullptr);
        Weed weed({0, 0, 0}, nullptr);
        Molly molly({0, 0, 0}, nullptr);
        
        // Should not crash
        adrenaline.Consume();
        salvia.Consume();
        cocaine.Consume();
        shrooms.Consume();
        vodka.Consume();
        weed.Consume();
        molly.Consume();
        
        REQUIRE(true);  // If we got here, Consume() didn't crash
    }
}

TEST_CASE("Substance - Stacking in Inventory", "[substance][inventory][regression]") {
    Inventory inventory;
    
    SECTION("Substances stack correctly") {
        Weed* weed1 = new Weed({0, 0, 0}, nullptr);
        Weed* weed2 = new Weed({1, 1, 1}, nullptr);
        
        inventory.AddItem(weed1);
        inventory.AddItem(weed2);
        
        // Both weeds should stack into one slot
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 2);
        
        // Both should point to the same item object (first one added)
        REQUIRE(inventory.GetStack(0)->item == weed1);
    }
    
    SECTION("Removing one from stack of two") {
        Weed* weed1 = new Weed({0, 0, 0}, nullptr);
        Weed* weed2 = new Weed({1, 1, 1}, nullptr);
        
        inventory.AddItem(weed1);
        inventory.AddItem(weed2);
        
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 2);
        
        // Save the item pointer
        Item* itemPtr = inventory.GetStack(0)->item;
        
        // Remove one
        inventory.RemoveItem(0);
        
        // Stack should still exist with count=1
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 1);
        
        // Item pointer should still be valid (same pointer)
        REQUIRE(inventory.GetStack(0)->item == itemPtr);
        
        // Clean up the remaining item
        delete itemPtr;
    }
    
    SECTION("Removing last one from stack deletes stack") {
        Weed* weed = new Weed({0, 0, 0}, nullptr);
        
        inventory.AddItem(weed);
        
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 1);
        
        // Remove the only one
        inventory.RemoveItem(0);
        
        // Stack should be gone
        REQUIRE(inventory.GetStackCount() == 0);
        
        // Clean up
        delete weed;
    }
    
    SECTION("Multiple different substances don't stack together") {
        Weed* weed = new Weed({0, 0, 0}, nullptr);
        Cocaine* cocaine = new Cocaine({1, 1, 1}, nullptr);
        
        inventory.AddItem(weed);
        inventory.AddItem(cocaine);
        
        // Should be two separate stacks
        REQUIRE(inventory.GetStackCount() == 2);
        REQUIRE(inventory.GetStack(0)->count == 1);
        REQUIRE(inventory.GetStack(1)->count == 1);
        
        // Clean up
        delete weed;
        delete cocaine;
    }
    
    SECTION("Three substances of same type stack correctly") {
        Molly* molly1 = new Molly({0, 0, 0}, nullptr);
        Molly* molly2 = new Molly({1, 1, 1}, nullptr);
        Molly* molly3 = new Molly({2, 2, 2}, nullptr);
        
        inventory.AddItem(molly1);
        inventory.AddItem(molly2);
        inventory.AddItem(molly3);
        
        // All three should stack
        REQUIRE(inventory.GetStackCount() == 1);
        REQUIRE(inventory.GetStack(0)->count == 3);
        
        // Remove one at a time
        inventory.RemoveItem(0);
        REQUIRE(inventory.GetStack(0)->count == 2);
        
        inventory.RemoveItem(0);
        REQUIRE(inventory.GetStack(0)->count == 1);
        
        inventory.RemoveItem(0);
        REQUIRE(inventory.GetStackCount() == 0);
        
        // Clean up
        delete molly1;
    }
}

TEST_CASE("Substance - CanStack returns true", "[substance]") {
    SECTION("All substances are stackable") {
        Adrenaline adrenaline({0, 0, 0}, nullptr);
        Salvia salvia({0, 0, 0}, nullptr);
        Cocaine cocaine({0, 0, 0}, nullptr);
        Shrooms shrooms({0, 0, 0}, nullptr);
        Vodka vodka({0, 0, 0}, nullptr);
        Weed weed({0, 0, 0}, nullptr);
        Molly molly({0, 0, 0}, nullptr);
        
        REQUIRE(adrenaline.CanStack() == true);
        REQUIRE(salvia.CanStack() == true);
        REQUIRE(cocaine.CanStack() == true);
        REQUIRE(shrooms.CanStack() == true);
        REQUIRE(vodka.CanStack() == true);
        REQUIRE(weed.CanStack() == true);
        REQUIRE(molly.CanStack() == true);
    }
}
