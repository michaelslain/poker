#include "catch_amalgamated.hpp"
#include "core/dom.hpp"
#include "core/object.hpp"

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

TEST_CASE("DOM - GetObjectByID (O(1) Hash Lookup)", "[dom]") {
    DOM dom;
    Object* obj1 = new Object({0, 0, 0});
    Object* obj2 = new Object({1, 1, 1});
    Object* obj3 = new Object({2, 2, 2});

    SECTION("Get single object by ID") {
        dom.AddObject(obj1);
        Object* found = dom.GetObjectByID(obj1->GetID());
        REQUIRE(found == obj1);
    }

    SECTION("Get multiple objects by ID in correct order") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        dom.AddObject(obj3);

        REQUIRE(dom.GetObjectByID(obj1->GetID()) == obj1);
        REQUIRE(dom.GetObjectByID(obj2->GetID()) == obj2);
        REQUIRE(dom.GetObjectByID(obj3->GetID()) == obj3);
    }

    SECTION("GetObjectByID returns nullptr for non-existent ID") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        Object* found = dom.GetObjectByID(99999);
        REQUIRE(found == nullptr);
    }

    SECTION("Hash map maintained after object removal") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        dom.AddObject(obj3);

        // Remove middle object
        dom.RemoveObject(obj2);

        // Other objects still findable
        REQUIRE(dom.GetObjectByID(obj1->GetID()) == obj1);
        REQUIRE(dom.GetObjectByID(obj3->GetID()) == obj3);

        // Removed object not findable
        REQUIRE(dom.GetObjectByID(obj2->GetID()) == nullptr);
    }

    SECTION("Hash map works after removing first object") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);
        dom.AddObject(obj3);

        dom.RemoveObject(obj1);

        // Remaining objects findable despite index shift
        REQUIRE(dom.GetObjectByID(obj2->GetID()) == obj2);
        REQUIRE(dom.GetObjectByID(obj3->GetID()) == obj3);
    }

    SECTION("Hash map cleared on Cleanup") {
        dom.AddObject(obj1);
        dom.AddObject(obj2);

        dom.Cleanup();

        // Both should be unfindable
        REQUIRE(dom.GetObjectByID(obj1->GetID()) == nullptr);
        REQUIRE(dom.GetObjectByID(obj2->GetID()) == nullptr);
    }

    delete obj1;
    delete obj2;
    delete obj3;
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
