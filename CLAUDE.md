# Poker Game - Technical Documentation

## Project Overview
A first-person poker game built with C++ and raylib using object-oriented architecture with inheritance, inventory system, DOM-based object management, and ODE physics engine integration.

**Visual Style**: Minimalist, flat-shaded with **no wireframe outlines**. All 3D geometry is rendered as solid colors without black edge lines.

---

## Build System

### Configuration
- **Compiler**: `g++ -std=c++17`
- **Flags**: `-Wall -Wextra -Isrc`
- **Libraries**: raylib, ODE (via Homebrew)
- **Paths**: `/opt/homebrew/opt/{raylib,ode}/{include,lib}`
- **Frameworks**: OpenGL, Cocoa, IOKit, CoreAudio, CoreVideo
- **Auto-detection**: Source files found via `$(shell find src -name '*.cpp')`
- **Caching**: Uses ccache for 2-5x faster debug builds

### Commands
```bash
make           # Release build (default, optimized)
make debug     # Debug build (fast compile, debug symbols)
make run       # Build and run (release)
make run-debug # Build and run (debug, recommended for dev)
make test      # Build and run all unit tests
make clean     # Remove build artifacts
```

---

## Testing (Catch2 v3.5.0)

**Run**: `make test`
- **36 test files** covering all game systems
- **202 test cases**, **1367 assertions**, **100% pass rate**
- Location: `tests/` directory
- Framework: Catch2 v3.5.0 (header-only)

### Test Structure
```cpp
#include "catch_amalgamated.hpp"
#include "category/class_name.hpp"

TEST_CASE("ClassName - Feature", "[tag]") {
    SECTION("Specific scenario") {
        // Arrange, Act, Assert
        REQUIRE(condition);
    }
}
```

### Key Test Files
- Core: `test_object`, `test_dom`, `test_physics`, `test_rigidbody`
- Entities: `test_player`, `test_enemy`, `test_dealer`, `test_person`
- Items: `test_card`, `test_chip`, `test_inventory`, `test_deck`
- Weapons: `test_pistol`, `test_weapon`
- Substances: `test_fent`, `test_substance`, `test_shrooms`
- World: `test_floor`, `test_wall`, `test_ceiling`, `test_stairs`
- Gameplay: `test_poker_table`, `test_level_generator`, `test_insanity_manager`
- Rendering: `test_light`, `test_lighting_manager`, `test_light_bulb`
- Scenes: `test_death_scene`, `test_hospital_scene`

---

## Architecture

### Class Hierarchy (Inheritance)
```
Object (base class)
├── Interactable
│   ├── Item (bridge to inventory)
│   │   ├── Card, Chip
│   │   ├── Weapon (abstract) → Pistol
│   │   └── Substance (abstract) → Weed, Cocaine, Molly, Adrenaline, Salvia, Shrooms, Vodka, Fent
│   └── PokerTable
├── Person (abstract, has physics & inventory)
│   ├── Player (human with camera)
│   ├── Enemy (AI)
│   └── Dealer (NPC)
├── Light (abstract) → LightBulb
├── Floor, Ceiling, Wall, Stairs
├── Plane, RigidBody, ChipStack, Spawner

Standalone Classes:
GameCamera, PhysicsWorld, DOM, Inventory, Deck, Collider, Scene,
SceneManager, LightingManager, PsychedelicManager, InsanityManager,
LevelManager, LevelGenerator, HospitalScene
```

### Directory Structure (File Organization)
```
poker/
├── src/
│   ├── core/         # object, dom, physics, rigidbody, collider, scene, level_manager
│   ├── entities/     # person, player, enemy, dealer
│   ├── items/        # interactable, item, card, chip, inventory, deck
│   ├── weapons/      # weapon, pistol
│   ├── substances/   # substance, weed, cocaine, molly, fent, etc.
│   ├── rendering/    # camera, light, light_bulb, lighting_manager, psychedelic_manager
│   ├── gameplay/     # poker_table, insanity_manager, level_generator
│   ├── world/        # floor, ceiling, wall, stairs, spawner
│   └── scenes/       # game_scene, death_scene, hospital_scene
├── tests/           # Catch2 v3.5.0 unit tests
├── shaders/         # lighting.vs/fs, psychedelic.vs/fs, vignette.vs/fs
├── main.cpp
└── Makefile
```

---

## Core Systems

### Object (Base Class)
- Public: `position`, `rotation`, `scale` (Vector3), `usesLighting` (bool)
- Private: `id` (unique), static `nextID`
- Virtual: `Update(dt)`, `Draw(camera)`, `GetType()`
- **Type system**: Hierarchical (e.g., `"object_interactable_item_card_hearts_ace"`)
- Check types with `type.find("keyword") != std::string::npos`

### DOM (Document Object Model)
- Global static instance: `DOM::SetGlobal()` / `DOM::GetGlobal()`
- Manages `std::vector<Object*>` (doesn't own objects)
- `AddObject()`, `RemoveObject()`, `Cleanup()` (clears vector)
- All scene objects tracked for updates/rendering

### PhysicsWorld (ODE Wrapper)
- Global static instance: `PhysicsWorld::SetGlobal()` / `GetGlobal()`
- Contains `dWorldID`, `dSpaceID`, `dJointGroupID`
- Constructor: Initializes ODE with gravity (0, -9.81, 0)
- `Step(dt)`: Advances simulation with **120Hz substepping** to prevent tunneling
- `NearCallback()`: Handles collisions with proper CFM/ERP (0.0/0.2)
- Contact mode: `dContactSoftCFM | dContactSoftERP | dContactApprox1`
- Friction: `mu = dInfinity` (no sliding)

---

## Entities

### Person (Abstract Base)
- **Physics**: All persons have ODE capsule physics
  - Capsule: radius=0.4m, `CAPSULE_HEIGHT = 3.4f` (scaled by height multiplier), mass=70kg (scaled by height)
  - **Rotated 90°** around X-axis (ODE capsules default to Z-axis, we need Y-axis for upright stance)
  - **Coordinate system**: `position.y` represents **feet position** (Y=0 at floor level)
  - Physics body center: `feetPosition + (CAPSULE_HEIGHT*height)/2`
  - Visual mesh extends upward from feet position, offset by `1.3*height` in rendering
- **Inventory**: `Inventory inventory` member
- **Seating**: `isSeated`, `seatPosition`, `standingYLevel`, `SitDown()`, `StandUp()`, `SitDownFacingPoint()`
  - `standingYLevel`: Saved Y position before sitting, restored when standing up
- **Rendering**: `usesLighting = false` (renders pitch black)
- Protected: `name`, `height`, `bodyYaw`, `body`, `geom`, `physics`, `debugColor`
- Public getters: `GetBody()`, `GetGeom()` for physics access
- Virtual: `PromptBet()` (returns 0=fold, 1=call, 2=raise)

### Player
- Inherits Person
- **Movement**: WASD (5.0 speed), mouse look (0.001 sensitivity)
- **Camera**: `GameCamera` instance at eye level (2.9*height above feet position)
- **Physics**: Horizontal force (500.0), velocity damping (0.99)
- **Spawn position**: Y=0.01 (feet slightly above floor to prevent penetration)
- **Inventory**: `selectedItemIndex`, `lastHeldItemIndex`
- **Insanity**: `InsanityManager insanityManager` (public), affects FOV (60°-150°)
- **Death**: `isDying`, `deathVignetteProgress`, `vignetteShader`, `TriggerDeath()`, `IsDead()`
- **Betting UI**: `bettingUIActive`, `bettingChoice`, `raiseSliderValue`
- **Card selection**: `cardSelectionUIActive`, `selectedCardIndices`
- **Global instance**: `Player::SetGlobal()` / `GetGlobal()` for substance access
- **Teleport**: `Teleport(newPos)` updates position, physics body, geometry, re-enables gravity
- **Constructor**: `Player(Vector3 pos, const std::string& name = "Player")` (physics from global)

### Enemy
- Inherits Person
- AI-controlled poker player
- Thinking timer: 2-4 seconds, then random fold/call/raise
- Height: 1.5x normal (visual height 2.7 units from feet)
- Members: `thinkingTimer`, `thinkingDuration`, `isThinking`, `pendingAction`
- Renders pitch black (default Person rendering)

### Dealer
- Inherits Person
- NPC positioned at poker table
- Visual presence only (no betting logic)
- Normal height (1.0x)
- Renders pitch black (default Person rendering)

---

## Items & Inventory

### Item (Abstract)
- Inherits Interactable
- Public: `usable` (bool) - can be used via left-click?
- Virtual: `Use()`, `DrawIcon()`, `DrawHeld(camera)`

### Card
- Inherits Item
- Enums: `Suit` (HEARTS, DIAMONDS, CLUBS, SPADES), `Rank` (ACE-KING)
- `RenderTexture2D texture` for card face
- Optional `RigidBody*` for physics
- `usesLighting = false` (manual lighting in Draw)
- Constructor: `Card(Suit, Rank, Vector3 pos, PhysicsWorld* physics = nullptr)`

### Chip
- Inherits Item
- Values: 1 (WHITE), 5 (RED), 10 (BLUE), 25 (GREEN), 100 (BLACK)
- `RenderTexture2D iconTexture`, optional `RigidBody*`
- `usesLighting = false`
- Constructor: `Chip(int value, Vector3 pos, PhysicsWorld* physics = nullptr)`

### Inventory
- `std::vector<ItemStack>` (Item*, count, typeString)
- `AddItem()`: Adds or increments stack, **auto-sorts**
- `RemoveItem(stackIndex)`: Decrements or removes
- **Sorting**: Weapons → Cards (by rank) → Chips (by value)
- `GetStackCount()`, `GetStack(index)`, `Cleanup()`

### Deck
- Inherits Object
- Creates all 52 cards (no physics)
- `Shuffle()` (Fisher-Yates), `DrawCard()`, `Peek()`, `Reset()`
- **Owns all cards** - never delete cards from community/inventory, deck reuses them

---

## Weapons & Substances

### Weapon (Abstract)
- Inherits Item
- `usable = true`, protected: `ammo`, `maxAmmo`, `rigidBody`
- `Use()`: Decrements ammo when left-clicked
- `PerformRaycast(start, dir, shooter)`: Returns hit `Person*` or `nullptr`
- Pure virtual: `Draw()`, `DrawIcon()`, `DrawHeld()`, `Clone()`
- Constructor: `Weapon(Vector3 pos, int ammo, int maxAmmo, PhysicsWorld* physics = nullptr)`

### Pistol
- Inherits Weapon
- 6-round revolver
- Constructor: `Pistol(Vector3 pos, PhysicsWorld* physics = nullptr)`

### Substance (Abstract)
- Inherits Item
- `usable = true`, protected: `rigidBody`, `color`
- `Use()` final: Calls `Consume()`, removes from inventory
- Pure virtual: `Consume()`, `GetName()`, `Clone()`
- Constructor: `Substance(Vector3 pos, Color color, PhysicsWorld* physics = nullptr)`

### Fent
- Inherits Substance
- Dark gray {50, 50, 50, 255}
- `Consume()`: Triggers instant death via `Player::GetGlobal()->TriggerDeath()`

---

## Physics Details

### Person Physics (Key Fix)
**Problem**: Capsules were sinking through floor because ODE capsules default to **Z-axis alignment** (horizontal), but we need **Y-axis alignment** (vertical) for upright characters.

**Solution**: Rotate capsule 90° around X-axis using `dGeomSetOffsetRotation()`
```cpp
dMatrix3 R;
dRFromAxisAndAngle(R, 1, 0, 0, M_PI / 2.0);  // 90° around X-axis
dGeomSetOffsetRotation(geom, R);
```

**Coordinate System**:
- **Feet position**: `position.y` represents where the person's feet are (Y=0 at floor level)
- Visual mesh: Rendered with offset of `1.3*height` upward from feet position
- Physics body center: `feetPosition + (CAPSULE_HEIGHT*height)/2`
- Example: Player spawns at Y=0.01 (feet just above floor), body center at ~Y=1.71
- **Safety net**: Person::Update() checks for floor clipping (Y < -0.01) and teleports back to Y=0

**Capsule Parameters**:
- Radius: 0.4m
- `CAPSULE_HEIGHT`: 3.4f (constant, scaled by height multiplier)
- Cylinder length: `CAPSULE_HEIGHT - 2*radius` (2.6m for standard height)
- Mass: 70kg (scaled by height multiplier)
- Direction: 2 (Y-axis in ODE, after rotation)

**Collision**:
- Category: `COLLISION_CATEGORY_PLAYER` (1 << 0)
- Collides with: All categories except items

### PhysicsWorld Parameters
- **Gravity**: 25.0 m/s² (increased from 9.81 for more responsive feel)
- **World CFM**: 1e-5 (low for stiff world)
- **World ERP**: 0.96 (high for strong correction, from working ODE examples)
- **Contact mode**: `dContactSoftCFM | dContactSoftERP | dContactApprox1`
- **Contact CFM**: 0.001 (very low for hard contacts, stable ground collision)
- **Contact ERP**: 0.8 (high for strong correction)
- **Max correcting vel**: 10.0 (increased from 0.1 for proper correction)
- **Surface layer**: 0.001 (small penetration allowed)
- **Friction**: `mu = dInfinity` (infinite friction, no sliding)
- **Bounce**: Removed (no `dContactBounce` flag)
- **Substepping**: 120Hz fixed timestep (1/120 = ~0.00833s) to prevent tunneling
- **Contact points**: 8 per collision (increased from 4 for better stability)

### Collision Categories
```cpp
#define COLLISION_CATEGORY_PLAYER   (1 << 0)
#define COLLISION_CATEGORY_ITEM     (1 << 1)
#define COLLISION_CATEGORY_TABLE    (1 << 2)
#define COLLISION_CATEGORY_WALL     (1 << 3)
#define COLLISION_CATEGORY_STAIRS   (1 << 4)
```

---

## Poker Table

### PokerTable
- Inherits Interactable
- Constructor: `PokerTable(Vector3 pos, Vector3 size, Color color)` (physics from global)
- **Seats**: 7 around table (dealer stands at bottom-center)
- **Game objects**: `dealer`, `deck`, `potStack`, `communityCards`
- **State**: `handActive`, `bettingActive`, `showdownActive`, `currentBet`, `potValue`
- **Blinds**: SB=5, BB=10
- **Flow**: `StartHand()` → `PostBlinds()` → `DealHoleCards()` → betting rounds → `Showdown()` → `EndHand()`
- **Hand evaluation**: `EvaluateHand()` returns `HandEvaluation` (HIGH_CARD to ROYAL_FLUSH)
- **Chip management**: `CountChips()`, `TakeChips()`, `GiveChips()`, `CalculateChipCombination()`

---

## Lighting System

### LightingManager (Static)
- `InitLightingSystem()`: Load shaders/lighting.vs/fs (call AFTER `InitWindow()`)
- `CleanupLightingSystem()`: Unload shader (call BEFORE `CloseWindow()`)
- `CreateLight(type, pos, target, color)`: Returns `RaylibLight` with unique index
- `UpdateLightValues(light)`: Send light data to shader
- `UpdateCameraPosition(pos)`: Update view uniform
- `ResetLights()`: **CRITICAL** - reset counter when cleaning up levels
- `MAX_LIGHTS = 32`, `LightType` enum (DIRECTIONAL=0, POINT=1)

### Light & LightBulb
- `Light`: Abstract base, `usesLighting = false`, virtual `UpdateLight()`
- `LightBulb`: Hanging lantern with **blueish tint** RGB(100, 120, 180)
- Renders decorative geometry (chain, fixture, glass, bulb, screw, glow halos)
- Must call `UpdateLight()` each frame to sync position

### Rendering Loop
```cpp
// Lit objects
BeginShaderMode(LightingManager::GetLightingShader());
for (Object* obj : litObjects) obj->Draw(camera);
EndShaderMode();

// Unlit objects (usesLighting = false)
for (Object* obj : unlitObjects) obj->Draw(camera);
```

---

## Psychedelic System

### PsychedelicManager (Static)
- `InitPsychedelicSystem()`: Load shaders/psychedelic.vs/fs
- `CleanupPsychedelicSystem()`: Unload shader
- `StartTrip(intensity)`: Begin 5-minute trip (0.0-1.0)
- `Update(dt)`: Auto-progresses through stages
- **Stages**: Come-up (0-60s) → Peak (60-180s) → Come-down (180-300s)
- Shrooms call `StartTrip(1.0f)` on consume

### Shader Effects
- Breathing/morphing, drifting/warping, geometric patterns
- Color shifting (HSV hue rotation, saturation boost)
- Spiral/tunnel effects, radial glow
- Simplex noise for organic warping

---

## Insanity & Death

### InsanityManager
- Tracks player mental state
- **Movement**: Decrease 0.3/s when moving, increase 0.01-0.02/s when still
- **Kills**: +0.2 `minInsanity` per kill, decays after 30s
- **Trips**: `insanity = tripIntensity + minInsanity`
- **FOV**: Interpolates 60° → 150° as insanity increases
- `DrawMeter()`: N64-style circular meter (yellow → red)

### Player Death
- `TriggerDeath()`: Starts 3-second vignette animation
- Triggered by: Insanity ≥ 100% or Fent overdose
- `Update()` returns early when `isDying = true`
- `IsDead()`: Returns true when `deathVignetteProgress ≥ 1.0`
- Main loop switches to death scene when `player->IsDead()`

---

## Level System

### LevelManager (Singleton)
- `GetInstance()`: Get singleton
- `NextLevel()`, `SetLevel(int)`, `JumpToLevel(int)`
- **Scaling**: `insanityMultiplier`, `minEnemiesPerTable`, `resourceSpawnRate`, `enemyAIQuality`
- **Dimensions**: `EnterAlternateDimension()`, `ExitAlternateDimension(jump)`, `IsInAlternateDimension()`
- Level 0 = Hospital, Level 1+ = Casino

### LevelGenerator
- **Algorithm**: Random walk on grid, rooms branch in 4 directions
- **Constraints**: Connecting rooms share dimension on connection axis
- **Rooms**: MIN_ROOMS=3, MAX_ROOMS=8, size=8-15 units
- **Contents**: First=empty, middle=60% poker table, last=stairs
- **Resources**: Chips, pistols, substances (scaled by difficulty)
- **Lighting**: 1 light bulb per room (max 32 lights)
- **Floor color**: Dark maroon RGB(20, 2, 2)

### HospitalScene
- Level 0 starting scene
- 15x15 room with floor, ceiling, 4 walls, light, stairs
- Spawn: (0, FLOOR_HEIGHT + 0.01, 0) - feet position just above floor

### Stairs
- Trigger level transitions via collision
- `CheckPlayerCollision(playerGeom)`: Returns true on first hit
- If in alt dimension: Exit with random jump (70% up, 20% same, 10% down)
- Otherwise: Progress to next level

---

## Controls
- **WASD**: Move
- **Mouse**: Look (0.001 sensitivity)
- **U**: Toggle cursor lock
- **E**: Interact with closest object
- **X**: Select/deselect inventory item
- **Left/Right Arrow**: Navigate inventory
- **[ ]**: Adjust FOV
- **C**: Toggle collision debug visualization
- **ESC**: Close window

### Debug Controls (ifdef DEBUG_HOTKEYS)
- **Ctrl+1-9**: Jump directly to level 1-9

---

## Common Patterns

### Creating Objects
```cpp
// Direct allocation
Card* card = new Card(SUIT_SPADES, RANK_ACE, {0, 2, 0});
dom.AddObject(card);

// Or use Spawner (auto-spawns on construction)
Spawner* spawner = new Spawner({0, 2, 0}, 2.0f, new Card(SUIT_HEARTS, RANK_KING, {0,0,0}), 5);
dom.AddObject(spawner);
```

### Cleanup
```cpp
for (int i = 0; i < dom.GetCount(); i++) {
    Object* obj = dom.GetObject(i);
    delete obj;  // Calls destructor (RAII)
}
dom.Cleanup();  // Clear vector
```

### Item Pickup
```cpp
Item* item = static_cast<Item*>(interactable);
player->GetInventory()->AddItem(item);
item->isActive = false;
dom.RemoveObject(item);
```

---

## Important Pitfalls

1. **Virtual Functions**: Always mark overrides with `override` keyword
2. **Physics Order**: Call `physics.Step()` before `Update()` calls
3. **DOM Ownership**: DOM doesn't own objects - must delete separately
4. **Card Ownership**: NEVER delete cards from community/inventory - Deck owns and reuses them
5. **Lighting Init Order**: `InitLightingSystem()` AFTER `InitWindow()`, BEFORE creating lights
6. **Lighting Cleanup**: `CleanupLightingSystem()` BEFORE `CloseWindow()`
7. **Light Reset**: Call `LightingManager::ResetLights()` when cleaning up levels (prevents "only one light works" bug)
8. **Deferred Cleanup**: Never clean up DOM during iteration - set flag, cleanup after loop
9. **Global Player**: Call `Player::SetGlobal(player)` after creating player
10. **Death System**: Death logic in Player class, not InsanityManager
11. **ODE Plane Colliders**: ODE planes don't use position - they use distance from origin along normal. For Floor, pass `position.y` as `offset.x` parameter: `collider.InitStatic(physics, COLLISION_SHAPE_PLANE, {0, 1, 0}, {position.y, 0, 0})`
12. **ODE Capsule Orientation**: ODE capsules default to Z-axis alignment (horizontal). For upright characters, rotate 90° around X-axis using `dGeomSetOffsetRotation()`. See Person constructor for implementation.
13. **Physics from Global**: Objects no longer take `PhysicsWorld*` parameter - they use `PhysicsWorld::GetGlobal()` internally. Same for DOM. Set globals in main: `PhysicsWorld::SetGlobal(&physics)`, `DOM::SetGlobal(&dom)`.
14. **Person Coordinate System**: `position.y` is **feet position** (Y=0 at floor). Spawn at Y=0.01, not Y=1.3. Visual mesh is offset upward during rendering.
15. **Level Transitions**: Check stairs collision BEFORE `physics.Step()` and use `continue` to skip rest of frame during level transition to prevent player from falling.
16. **Player Physics During Cleanup**: Disable gravity with `dBodySetGravityMode(body, 0)` when cleaning up levels. Re-enable in `Teleport()` with `dBodySetGravityMode(body, 1)`.
17. **Teleport Geometry**: Always update both `dBodySetPosition()` AND `dGeomSetPosition()` when teleporting - geometry position is used for collision detection.
18. **Seating Y Position**: When sitting/standing, Person now saves/restores `standingYLevel` to preserve vertical position across seating changes.

---

## Development Setup

### Prerequisites
```bash
brew install raylib ode
```

### IDE (Zed)
`.clangd` configured with raylib/ODE include paths and `-I/Users/michaelslain/Documents/dev/poker/src`

---

## Main Game Loop
```cpp
// Init
PhysicsWorld physics;
DOM dom;
DOM::SetGlobal(&dom);
PhysicsWorld::SetGlobal(&physics);

// NEW SYSTEM: Spawn at feet position (Y=0.01, just above floor)
Player* player = new Player({0, 0.01, 0});
Player::SetGlobal(player);
dom.AddObject(player);

// Level
LevelGenerator levelGen(&physics, &dom);
levelGen.GenerateLevel(1);

// Game loop
while (!WindowShouldClose()) {
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    // Check stairs collision BEFORE physics - prevents falling during transition
    bool stairsTriggered = false;
    for (int i = 0; i < dom.GetCount(); i++) {
        if (TypeContains(dom.GetObject(i)->GetType(), "stairs")) {
            Stairs* stairs = static_cast<Stairs*>(dom.GetObject(i));
            if (stairs->CheckPlayerCollision(player->GetGeom())) {
                // Handle level transition, clean up, regenerate
                stairsTriggered = true;
                break;
            }
        }
    }
    if (stairsTriggered) {
        // Clean up and regenerate level
        continue;  // Skip rest of frame
    }

    // Update
    physics.Step(dt);
    for (int i = 0; i < dom.GetCount(); i++) {
        dom.GetObject(i)->Update(dt);
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    Camera3D* cam = player->GetCamera();
    BeginMode3D(*cam);
        // Render lit objects with shader, then unlit objects
        for (int i = 0; i < dom.GetCount(); i++) {
            dom.GetObject(i)->Draw(*cam);
        }
    EndMode3D();

    player->DrawInventoryUI();
    EndDrawing();

    // Check death
    if (player->IsDead()) {
        // Switch to death scene
    }
}

// Cleanup
for (int i = 0; i < dom.GetCount(); i++) delete dom.GetObject(i);
dom.Cleanup();
LightingManager::CleanupLightingSystem();
PsychedelicManager::CleanupPsychedelicSystem();
```
