#ifndef COLLISION_CATEGORIES_HPP
#define COLLISION_CATEGORIES_HPP

// Collision categories for ODE physics
// Each category is a unique bit flag

#define COLLISION_CATEGORY_WORLD    (1 << 0)  // Static world geometry (floors, walls, ceilings)
#define COLLISION_CATEGORY_PERSON   (1 << 1)  // People (player, enemies, dealer)
#define COLLISION_CATEGORY_ITEM     (1 << 2)  // Pickupable items (cards, chips, weapons, substances)
#define COLLISION_CATEGORY_TABLE    (1 << 3)  // Poker tables
#define COLLISION_CATEGORY_STAIRS   (1 << 4)  // Stairs (level transitions)

// Collision masks define what each category collides with
// Format: Category collides with (bitmask)

// WORLD: Collides with everything except other WORLD geometry
#define COLLISION_MASK_WORLD    (~COLLISION_CATEGORY_WORLD)

// PERSON: Collides with WORLD, TABLES, and STAIRS only (not other people or items)
#define COLLISION_MASK_PERSON   (COLLISION_CATEGORY_WORLD | COLLISION_CATEGORY_TABLE | COLLISION_CATEGORY_STAIRS)

// ITEM: Collides with WORLD and other ITEMS only (not people or tables)
#define COLLISION_MASK_ITEM     (COLLISION_CATEGORY_WORLD | COLLISION_CATEGORY_ITEM)

// TABLE: Collides with WORLD and PERSONS only
#define COLLISION_MASK_TABLE    (COLLISION_CATEGORY_WORLD | COLLISION_CATEGORY_PERSON)

// STAIRS: Collides with PERSONS only (for triggering level transitions)
#define COLLISION_MASK_STAIRS   (COLLISION_CATEGORY_PERSON)

#endif // COLLISION_CATEGORIES_HPP
