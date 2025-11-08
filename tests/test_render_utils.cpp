#include "catch_amalgamated.hpp"
#include "../include/render_utils.hpp"

TEST_CASE("RenderUtils - DrawTextBillboard", "[render_utils]") {
    Camera3D camera = {0};
    camera.position = {0, 2, -5};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    SECTION("Draw text billboard doesn't crash") {
        DrawTextBillboard(camera, "Test", {0, 1, 0}, 20, BLACK);
        REQUIRE(true);
    }
    
    SECTION("Draw empty string") {
        DrawTextBillboard(camera, "", {0, 1, 0}, 20, BLACK);
        REQUIRE(true);
    }
}

TEST_CASE("RenderUtils - DrawText3D", "[render_utils]") {
    SECTION("Draw 3D text doesn't crash") {
        DrawText3D("Test", {0, 0, 0}, 20, BLACK);
        REQUIRE(true);
    }
    
    SECTION("Draw empty string") {
        DrawText3D("", {0, 0, 0}, 20, BLACK);
        REQUIRE(true);
    }
}
