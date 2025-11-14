#include "catch_amalgamated.hpp"
#include "../include/substance.hpp"
#include "../include/adrenaline.hpp"
#include "../include/salvia.hpp"
#include "../include/cocaine.hpp"
#include "../include/shrooms.hpp"
#include "../include/vodka.hpp"
#include "../include/weed.hpp"
#include "../include/molly.hpp"

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
