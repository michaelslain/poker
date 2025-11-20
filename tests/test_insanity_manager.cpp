#include "catch_amalgamated.hpp"
#include "gameplay/insanity_manager.hpp"
#include "rendering/psychedelic_manager.hpp"
#include "raymath.h"

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

TEST_CASE("InsanityManager - Salvia peak forces 100% insanity", "[insanity_manager][salvia]") {
    // Note: These tests verify the Salvia peak detection logic
    // PsychedelicManager is a static class that would require shader initialization
    // So we test the InsanityManager's response to trip parameters
    
    InsanityManager manager({0, 0, 0});
    
    SECTION("Salvia trip during come-up phase uses normal trip intensity") {
        // During come-up (< 5 seconds), should use trip intensity normally
        // This is tested indirectly - the peak logic only activates after 5s
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        // Should be normal trip intensity + min insanity
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.5f));
    }
    
    SECTION("Normal (non-Salvia) trip respects trip intensity") {
        // Shrooms trip at 0.6 intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.6f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.6f));
        
        // Even with higher intensity, follows trip value
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.8f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.8f));
    }
    
    SECTION("Trip intensity combines with minimum insanity") {
        manager.OnKill(); // minInsanity = 0.2
        
        // Trip at 0.4 intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.4f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.6f)); // 0.4 + 0.2
    }
}

TEST_CASE("InsanityManager - Regression: trip intensity edge cases", "[insanity_manager][regression]") {
    InsanityManager manager({0, 0, 0});
    
    SECTION("Zero trip intensity sets insanity to minimum") {
        manager.OnKill(); // minInsanity = 0.2
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.0f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.2f));
    }
    
    SECTION("Trip intensity of 1.0 with no min insanity") {
        manager.Update(1.0f, {0, 0, 0}, false, true, 1.0f);
        REQUIRE(manager.GetInsanity() == 1.0f);
    }
    
    SECTION("Switching between tripping and not tripping") {
        // Start trip
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.7f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.7f));
        
        // Stop trip, stand still
        manager.Update(1.0f, {0, 0, 0}, false, false, 0.0f);
        // Should increase by standing still rate (0.02)
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.72f));
        
        // Resume trip at different intensity
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        REQUIRE(manager.GetInsanity() == Catch::Approx(0.5f));
    }
}

TEST_CASE("InsanityManager - Salvia Gradual Increase", "[insanity_manager][salvia][regression]") {
    // Setup
    PsychedelicManager::CleanupPsychedelicSystem();
    PsychedelicManager::InitPsychedelicSystem();
    
    SECTION("Salvia peak gradually increases insanity to 100%") {
        PsychedelicManager::StopTrip();  // Ensure clean state
        InsanityManager manager({0, 0, 0});
        
        // Start Salvia trip
        PsychedelicManager::StartTrip(0.5f, TripType::SALVIA);
        
        // During come-up (first 5 seconds), insanity should follow trip intensity
        for (int i = 0; i < 5; i++) {
            PsychedelicManager::Update(1.0f);
            float tripIntensity = PsychedelicManager::GetCurrentIntensity();
            manager.Update(1.0f, {0, 0, 0}, false, true, tripIntensity);
        }
        
        float insanityAfterComeUp = manager.GetInsanity();
        REQUIRE(insanityAfterComeUp < 1.0f);  // Should not be at 100% yet
        
        // After peak starts, insanity should gradually increase
        PsychedelicManager::Update(1.0f);  // 6 seconds total
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        float insanityAt6s = manager.GetInsanity();
        
        // Should increase by 0.15 per second during peak
        REQUIRE(insanityAt6s > insanityAfterComeUp);
        REQUIRE(insanityAt6s == Catch::Approx(insanityAfterComeUp + 0.15f).margin(0.01f));
        
        // Advance several more seconds
        for (int i = 0; i < 10; i++) {
            PsychedelicManager::Update(1.0f);
            manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        }
        
        // Should reach 100% (clamped)
        REQUIRE(manager.GetInsanity() == Catch::Approx(1.0f));
        
        PsychedelicManager::StopTrip();  // Clean up
    }
    
    SECTION("Insanity increase rate is 0.15 per second") {
        PsychedelicManager::StopTrip();  // Ensure clean state
        InsanityManager manager({0, 0, 0});
        
        // Start Salvia trip and advance to peak
        PsychedelicManager::StartTrip(0.5f, TripType::SALVIA);
        for (int i = 0; i < 6; i++) {
            PsychedelicManager::Update(1.0f);
            manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        }
        
        // At this point, insanity has been increasing during peak
        float startInsanity = manager.GetInsanity();
        
        // Update for 1 second during peak
        PsychedelicManager::Update(1.0f);
        manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        
        // Should increase by approximately 0.15 (or reach 1.0 if already high)
        float endInsanity = manager.GetInsanity();
        float actualIncrease = endInsanity - startInsanity;
        
        // Either increased by ~0.15, or was clamped at 1.0
        if (endInsanity < 1.0f) {
            REQUIRE(actualIncrease >= 0.14f);
            REQUIRE(actualIncrease <= 0.16f);
        } else {
            REQUIRE(endInsanity == Catch::Approx(1.0f));
        }
        
        PsychedelicManager::StopTrip();  // Clean up
    }
    
    SECTION("Insanity clamped at 100% during Salvia peak") {
        PsychedelicManager::StopTrip();  // Ensure clean state
        InsanityManager manager({0, 0, 0});
        
        // Start with high insanity
        manager.Update(0.0f, {0, 0, 0}, false, true, 0.95f);
        
        // Start Salvia and advance to peak
        PsychedelicManager::StartTrip(0.5f, TripType::SALVIA);
        for (int i = 0; i < 6; i++) {
            PsychedelicManager::Update(1.0f);
        }
        
        // Update during peak - should clamp at 1.0
        for (int i = 0; i < 5; i++) {
            manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
            REQUIRE(manager.GetInsanity() <= 1.0f);  // Never exceeds 100%
        }
        
        REQUIRE(manager.GetInsanity() == Catch::Approx(1.0f));
        
        PsychedelicManager::StopTrip();  // Clean up
    }
    
    SECTION("Salvia come-down stops gradual increase") {
        PsychedelicManager::StopTrip();  // Ensure clean state
        InsanityManager manager({0, 0, 0});
        
        // Start Salvia and advance to peak
        PsychedelicManager::StartTrip(0.5f, TripType::SALVIA);
        for (int i = 0; i < 8; i++) {
            PsychedelicManager::Update(1.0f);
            manager.Update(1.0f, {0, 0, 0}, false, true, 0.5f);
        }
        
        float insanityDuringPeak = manager.GetInsanity();
        
        // Trigger come-down
        PsychedelicManager::TriggerComeDown();
        PsychedelicManager::Update(1.0f);
        
        // During come-down, should follow trip intensity (decreasing)
        float comeDownIntensity = PsychedelicManager::GetCurrentIntensity();
        manager.Update(1.0f, {0, 0, 0}, false, true, comeDownIntensity);
        
        // Should be following intensity, not continuing to increase
        REQUIRE(manager.GetInsanity() < insanityDuringPeak);
        
        PsychedelicManager::StopTrip();  // Clean up
    }
    
    // Final cleanup
    PsychedelicManager::StopTrip();
    PsychedelicManager::CleanupPsychedelicSystem();
}
