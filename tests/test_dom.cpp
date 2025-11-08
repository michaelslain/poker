#include "catch_amalgamated.hpp"
#include "../include/dom.hpp"
#include "../include/object.hpp"

TEST_CASE("DOM - Construction", "[dom]") {
    DOM dom;
    
    SECTION("New DOM is empty") {
        REQUIRE(dom.GetCount() == 0);
        REQUIRE(dom.GetObjects().size() == 0);
    }
}

TEST_CASE("DOM - Add Objects", "[dom]") {
    DOM dom;
    Object* obj1 = new Object({0, 0, 0});
    Object* obj2 = new Object({1, 1, 1});
    
    SECTION("Add single object") {
        dom.AddObject(obj1);
        REQUIRE(dom.GetCount() == 1);
        REQUIRE(dom.GetObject(0) == obj1);
    }
    
    SECTION("Add multiple objects") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        REQUIRE(dom.GetCount() == 2);
        REQUIRE(dom.GetObject(0) == obj1);
        REQUIRE(dom.GetObject(1) == obj2);
    }
    
    delete obj1;
    delete obj2;
}

TEST_CASE("DOM - Remove Objects", "[dom]") {
    DOM dom;
    Object* obj1 = new Object({0, 0, 0});
    Object* obj2 = new Object({1, 1, 1});
    Object* obj3 = new Object({2, 2, 2});
    
    SECTION("Remove object from middle") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        dom.AddObject(obj3);
        dom.RemoveObject(obj2);
        REQUIRE(dom.GetCount() == 2);
        REQUIRE(dom.GetObject(0) == obj1);
        REQUIRE(dom.GetObject(1) == obj3);
    }
    
    SECTION("Remove first object") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        dom.RemoveObject(obj1);
        REQUIRE(dom.GetCount() == 1);
        REQUIRE(dom.GetObject(0) == obj2);
    }
    
    delete obj1;
    delete obj2;
    delete obj3;
}

TEST_CASE("DOM - FindObjectByID", "[dom]") {
    DOM dom;
    Object* obj1 = new Object({0, 0, 0});
    Object* obj2 = new Object({1, 1, 1});
    
    dom.AddObject(obj1);
    dom.AddObject(obj2);
    
    SECTION("Find existing object") {
        Object* found = dom.FindObjectByID(obj1->GetID());
        REQUIRE(found == obj1);
    }
    
    SECTION("Find non-existent object returns nullptr") {
        Object* found = dom.FindObjectByID(99999);
        REQUIRE(found == nullptr);
    }
    
    delete obj1;
    delete obj2;
}

TEST_CASE("DOM - Cleanup", "[dom]") {
    DOM dom;
    Object* obj1 = new Object();
    Object* obj2 = new Object();
    
    dom.AddObject(obj1);
    dom.AddObject(obj2);
    
    dom.Cleanup();
    REQUIRE(dom.GetCount() == 0);
    
    delete obj1;
    delete obj2;
}

TEST_CASE("DOM - Global Instance", "[dom]") {
    // Save original global DOM (set by test_main.cpp)
    DOM* originalGlobal = DOM::GetGlobal();
    
    DOM dom1;
    
    SECTION("Set and get global DOM") {
        DOM::SetGlobal(&dom1);
        REQUIRE(DOM::GetGlobal() == &dom1);
    }
    
    SECTION("Global can be changed") {
        DOM dom2;
        DOM::SetGlobal(&dom1);
        DOM::SetGlobal(&dom2);
        REQUIRE(DOM::GetGlobal() == &dom2);
    }
    
    // Restore original global DOM after test
    DOM::SetGlobal(originalGlobal);
}
