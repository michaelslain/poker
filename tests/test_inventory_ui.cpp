#include "catch_amalgamated.hpp"
#include "rendering/inventory_ui.hpp"
#include "items/inventory.hpp"

TEST_CASE("InventoryUI - Draw with empty inventory", "[inventory_ui]") {
    Inventory inv;
    
    SECTION("Draw with no items doesn't crash") {
        InventoryUI_Draw(&inv, -1);
        REQUIRE(true);
    }
}

TEST_CASE("InventoryUI - Draw with selected index", "[inventory_ui]") {
    Inventory inv;
    
    SECTION("Draw with selection") {
        InventoryUI_Draw(&inv, 0);
        REQUIRE(true);
    }
    
    SECTION("Draw with negative selection") {
        InventoryUI_Draw(&inv, -1);
        REQUIRE(true);
    }
}
