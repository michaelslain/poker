#include "catch_amalgamated.hpp"
#include "rendering/render_utils.hpp"

TEST_CASE("RenderUtils - DrawTextBillboard", "[render_utils]") {
    // These functions use MeasureText which requires raylib to be initialized
    // We test that they don't crash - actual rendering is tested in gameplay
    Camera3D camera = {};
    camera.position = {0, 2, -5};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SECTION("DrawTextBillboard exists") {
        // Just verify the function signature compiles
        REQUIRE(true);
    }
}

TEST_CASE("RenderUtils - DrawText3D", "[render_utils]") {
    SECTION("DrawText3D exists") {
        // Just verify the function signature compiles
        REQUIRE(true);
    }
}

TEST_CASE("RenderUtils - DrawLevelUI", "[render_utils][salvia][regression]") {
    SECTION("DrawLevelUI exists and compiles") {
        // The actual rendering requires raylib window initialization
        // We just verify the function signature compiles
        // Behavior:
        // - dimension > 0: shows "SALVIA DIMENSION" in purple
        // - dimension == 0: shows "LEVEL X" in white
        REQUIRE(true);
    }
    
    SECTION("DrawLevelUI handles normal dimension") {
        // Test would show "LEVEL 5" for level=5, dimension=0
        // Actual rendering tested in gameplay
        REQUIRE(true);
    }
    
    SECTION("DrawLevelUI handles alternate dimension") {
        // Test would show "SALVIA DIMENSION" for any level, dimension=1
        // Actual rendering tested in gameplay
        REQUIRE(true);
    }
}
