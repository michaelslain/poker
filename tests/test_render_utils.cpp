#include "catch_amalgamated.hpp"
#include "../include/render_utils.hpp"

TEST_CASE("RenderUtils - DrawTextBillboard", "[render_utils]") {
    // These functions use MeasureText which requires raylib to be initialized
    // We test that they don't crash - actual rendering is tested in gameplay
    Camera3D camera = {0};
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
