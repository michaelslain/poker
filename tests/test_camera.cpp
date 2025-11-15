#include "catch_amalgamated.hpp"
#include "rendering/camera.hpp"

TEST_CASE("GameCamera - Construction", "[camera]") {
    SECTION("Create camera with default position") {
        GameCamera cam;
        REQUIRE(cam.camera.fovy == 45.0f);
        REQUIRE(cam.camera.projection == CAMERA_PERSPECTIVE);
    }
    
    SECTION("Create camera at custom position") {
        GameCamera cam({5, 10, 15});
        REQUIRE(cam.camera.position.x == 5.0f);
        REQUIRE(cam.camera.position.y == 10.0f);
        REQUIRE(cam.camera.position.z == 15.0f);
    }
}

TEST_CASE("GameCamera - Angle", "[camera]") {
    GameCamera cam;
    
    SECTION("Default angle is zero") {
        REQUIRE(cam.angle.x == 0.0f); // pitch
        REQUIRE(cam.angle.y == 0.0f); // yaw
    }
}

TEST_CASE("GameCamera - GetCamera", "[camera]") {
    GameCamera cam;
    Camera3D* camera = cam.GetCamera();
    REQUIRE(camera != nullptr);
}

TEST_CASE("GameCamera - SetTarget", "[camera]") {
    GameCamera cam;
    cam.SetTarget({10, 5, 3});
    // SetTarget actually sets the camera position, not target
    REQUIRE(cam.camera.position.x == 10.0f);
    REQUIRE(cam.camera.position.y == 5.0f);
    REQUIRE(cam.camera.position.z == 3.0f);
}
