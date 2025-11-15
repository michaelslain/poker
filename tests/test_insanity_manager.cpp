#include "catch_amalgamated.hpp"
#include "../include/insanity_manager.hpp"

TEST_CASE("InsanityManager - Construction", "[insanity_manager]") {
    SECTION("Default constructor initializes to zero") {
        InsanityManager manager({0, 0, 0});
        REQUIRE(manager.GetInsanity() == 0.0f);
        REQUIRE(manager.GetMinInsanity() == 0.0f);
    }
}

TEST_CASE("InsanityManager - Normal insanity system", "[insanity_manager]") {
    InsanityManager manager({0, 0, 0});
    
    SECTION("Standing still increases insanity") {
        // Standing still (not seated, not tripping)
        manager.Update(1.0f, {0, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetInsanity() > 0.0f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.02f)); // 0.02/sec when standing
    }
    
    SECTION("Seated increases insanity slower") {
        // Seated still (seated, not tripping)
        manager.Update(1.0f, {0, 0, 0}, true, false, 0.0f);
        REQUIRE(manager.GetInsanity() > 0.0f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.01f)); // 0.01/sec when seated
    }
    
    SECTION("Moving decreases insanity") {
        // First increase insanity
        manager.Update(10.0f, {0, 0, 0}, false, false, 0.0f);
        float initialInsanity = manager.GetInsanity();
        
        // Then move (position changes significantly)
        manager.Update(1.0f, {5, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetInsanity() < initialInsanity);
    }
    
    SECTION("Insanity clamps to [0.0, 1.0]") {
        // Increase way beyond 1.0
        manager.Update(100.0f, {0, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetInsanity() <= 1.0f);
        
        // Decrease way below 0.0
        for (int i = 0; i < 100; i++) {
            manager.Update(1.0f, {(float)i, 0, 0}, false, false, 0.0f);
        }
        REQUIRE(manager.GetInsanity() >= 0.0f);
    }
}

TEST_CASE("InsanityManager - Kill system", "[insanity_manager]") {
    InsanityManager manager({0, 0, 0});
    
    SECTION("First kill increases minimum insanity") {
        manager.OnKill();
        REQUIRE(manager.GetMinInsanity() == Catch::Approx(0.2f));
        REQUIRE(manager.GetInsanity() >= 0.2f);
    }
    
    SECTION("Multiple kills stack minimum insanity") {
        manager.OnKill();
        manager.OnKill();
        manager.OnKill();
        REQUIRE(manager.GetMinInsanity() == Catch::Approx(0.6f));
    }
    
    SECTION("Minimum insanity clamps at 1.0") {
        for (int i = 0; i < 10; i++) {
            manager.OnKill();
        }
        REQUIRE(manager.GetMinInsanity() <= 1.0f);
        REQUIRE(manager.GetMinInsanity() == 1.0f);
    }
    
    SECTION("Insanity cannot drop below minimum") {
        manager.OnKill(); // minInsanity = 0.2, timer = 30s
        
        // Try to decrease insanity by moving for 10 seconds (within hold time)
        for (int i = 0; i < 10; i++) {
            manager.Update(1.0f, {(float)i * 10, 0, 0}, false, false, 0.0f);
        }
        
        REQUIRE(manager.GetInsanity() >= manager.GetMinInsanity());
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.2f));
        REQUIRE(manager.GetMinInsanity() == Catch::Approx(0.2f)); // Still at floor
    }
    
    SECTION("Minimum insanity holds for 30 seconds then decays") {
        manager.OnKill(); // minInsanity = 0.2, timer = 30s
        
        // Increase insanity by standing still for 10 seconds
        manager.Update(10.0f, {0, 0, 0}, false, false, 0.0f);
        
        // Wait 20 more seconds (total 30s) - timer should expire but no decay yet
        manager.Update(20.0f, {0, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetMinInsanity() == Catch::Approx(0.2f));
        
        // Wait another 2 seconds - should decay by 0.05 * 2 = 0.1
        manager.Update(2.0f, {0, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetMinInsanity() < 0.2f);
        REQUIRE(manager.GetMinInsanity() == Catch::Approx(0.1f).margin(0.01f));
    }
}

TEST_CASE("InsanityManager - Psychedelic trip integration", "[insanity_manager]") {
    InsanityManager manager({0, 0, 0});
    
    SECTION("Trip intensity overrides normal insanity") {
        // Build up some normal insanity
        manager.Update(10.0f, {0, 0, 0}, false, false, 0.0f);
        
        // Start tripping at 0.5 intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.5f));
    }
    
    SECTION("Trip intensity adds to minimum insanity floor") {
        manager.OnKill(); // minInsanity = 0.2
        
        // Trip at 0.3 intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.3f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.5f)); // 0.3 + 0.2
    }
    
    SECTION("Trip intensity + min insanity clamps at 1.0") {
        // 3 kills = 0.6 min insanity
        manager.OnKill();
        manager.OnKill();
        manager.OnKill();
        
        // Trip at 0.8 intensity (would be 1.4 total)
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.8f);
        REQUIRE(manager.GetInsanity() <= 1.0f);
        REQUIRE(manager.GetInsanity() == 1.0f);
    }
    
    SECTION("After trip ends, normal insanity resumes") {
        // Trip at 0.7 intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.7f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.7f));
        
        // Stop tripping, stand still
        manager.Update(1.0f, {0, 0, 0}, false, false, 0.0f);
        // Should have normal insanity increase (0.02/sec standing)
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.72f));
    }
}

TEST_CASE("InsanityManager - Edge cases", "[insanity_manager]") {
    SECTION("Zero delta time does nothing") {
        InsanityManager manager({0, 0, 0});
        manager.Update(0.0f, {0, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetInsanity() == 0.0f);
    }
    
    SECTION("Very small movement doesn't count as moving") {
        InsanityManager manager({0, 0, 0});
        // Move less than threshold (0.01)
        manager.Update(1.0f, {0, 0, 0}, false, false, 0.0f);
        manager.Update(1.0f, {0.005f, 0, 0}, false, false, 0.0f);
        
        // Should still increase (standing still)
        REQUIRE(manager.GetInsanity() > 0.02f);
    }
    
    SECTION("Movement exactly at threshold counts as moving") {
        InsanityManager manager({0, 0, 0});
        // Build up some insanity
        manager.Update(10.0f, {0, 0, 0}, false, false, 0.0f);
        float initial = manager.GetInsanity();
        
        // Move exactly at threshold
        manager.Update(1.0f, {0.011f, 0, 0}, false, false, 0.0f);
        REQUIRE(manager.GetInsanity() < initial);
    }
}
