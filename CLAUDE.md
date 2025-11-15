## Project Overview
A first-person poker game built with C++ and raylib using object-oriented architecture with inheritance, inventory system, DOM-based object management, and ODE physics engine integration.

## Build System
The project uses a Makefile with macOS-specific paths:
- Raylib include path: `/opt/homebrew/opt/raylib/include`
- Raylib library path: `/opt/homebrew/opt/raylib/lib`
- ODE include path: `/opt/homebrew/opt/ode/include`
- ODE library path: `/opt/homebrew/opt/ode/lib`
- Compiler: `g++` with `-std=c++17`
- Compiler flags: `-Wall -Wextra -std=c++17 -Isrc`
- Target executable: `game`
- Links against: `-lraylib -lode -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo`
- **Auto-detection**: Source files automatically detected via `$(shell find src -name '*.cpp')`

### Build Commands
```bash
make           # Compile the project in release mode (default)
make debug     # Compile in debug mode (fast compilation, debug symbols)
make release   # Compile in release mode (optimized)
make run       # Build and run the game in release mode
make run-debug # Build and run the game in debug mode (recommended for development)
make test      # Build and run all unit tests (always uses debug mode)
make clean     # Remove build artifacts (*.o and game executable)
make ccache-stats  # Show compilation cache statistics
make ccache-clear  # Clear compilation cache
```

**Note**: The build system uses ccache for compilation caching and parallel compilation for speed. Debug builds are 2-5x faster to compile than release builds.

## Testing

The project uses the Catch2 v3.5.0 testing framework for comprehensive unit testing of all classes.

### Running Tests

```bash
make test      # Build and run all tests
```

This command compiles all test files and executes them, showing which tests pass or fail.

### Test Structure

- **Location**: All tests are in the `tests/` directory
- **Framework**: Catch2 v3.5.0 (header-only)
- **Test files**: One `test_*.cpp` file for each class (e.g., `test_card.cpp`, `test_player.cpp`)
- **Coverage**: 28 test files covering all game classes

### Test Organization

Each test file follows this structure:

```cpp
#include "catch_amalgamated.hpp"
#include "category/class_name.hpp"

TEST_CASE("ClassName - Feature", "[tag]") {
    SECTION("Specific test scenario") {
        // Arrange
        ClassName obj;

        // Act
        obj.DoSomething();

        // Assert
        REQUIRE(obj.GetValue() == expectedValue);
    }
}
```

### Available Test Files

- `test_object.cpp` - Base Object class
- `test_camera.cpp` - GameCamera functionality
- `test_card.cpp` - Card class with all suits/ranks
- `test_chip.cpp` - Chip values and colors
- `test_chip_stack.cpp` - Chip stack management
- `test_deck.cpp` - Deck shuffling and dealing
- `test_inventory.cpp` - Inventory stacking and management (includes regression tests)
- `test_dom.cpp` - DOM object tracking
- `test_interactable.cpp` - Interaction system
- `test_item.cpp` - Item base class
- `test_weapon.cpp` - Weapon base class
- `test_pistol.cpp` - Pistol weapon
- `test_person.cpp` - Person base class
- `test_enemy.cpp` - AI enemy behavior
- `test_dealer.cpp` - Dealer NPC

- `test_poker_table.cpp` - Poker table game logic
- `test_spawner.cpp` - Object spawning
- `test_floor.cpp` - Floor geometry
- `test_ceiling.cpp` - Ceiling geometry
- `test_wall.cpp` - Wall geometry

- `test_light.cpp` - Light base class
- `test_lighting_manager.cpp` - Lighting system management
- `test_light_bulb.cpp` - Light bulb rendering
- `test_physics.cpp` - ODE physics integration
- `test_rigidbody.cpp` - Physics body simulation
- `test_inventory_ui.cpp` - Inventory rendering
- `test_render_utils.cpp` - 3D text rendering
- `test_insanity_manager.cpp` - Insanity system (movement, kills, psychedelic integration)
- `test_death_scene.cpp` - Death scene (factory function, rendering, memory management)

### Test Categories

Tests are tagged for easy filtering:

- `[card]` - Card-related tests
- `[chip]` - Chip-related tests
- `[inventory]` - Inventory system tests
- `[regression]` - Tests for previously fixed bugs
- `[physics]` - Physics simulation tests
- `[dom]` - DOM management tests
- etc.

### Benefits of Testing

1. **Catch bugs early**: Find issues before gameplay testing
2. **Regression prevention**: Ensure fixed bugs stay fixed
3. **Documentation**: Tests show how classes should be used
4. **Refactoring safety**: Change code confidently knowing tests will catch breaks
5. **Edge case coverage**: Test unusual inputs and boundary conditions

### Writing New Tests

When adding a new class:

1. Create `tests/test_classname.cpp`
2. Include Catch2 and your class header
3. Write TEST_CASE blocks for each feature
4. Add test file to `TEST_SRCS` in Makefile
5. Run `make test` to verify

Example:

```cpp
#include "catch_amalgamated.hpp"
#include "category/my_class.hpp"

TEST_CASE("MyClass - Construction", "[myclass]") {
    SECTION("Default constructor") {
        MyClass obj;
        REQUIRE(obj.GetValue() == 0);
    }
}

TEST_CASE("MyClass - Operations", "[myclass]") {
    MyClass obj;

    SECTION("Setting value") {
        obj.SetValue(42);
        REQUIRE(obj.GetValue() == 42);
    }

    SECTION("Edge case - negative value") {
        obj.SetValue(-5);
        REQUIRE(obj.GetValue() == -5);
    }
}
```

### Regression Tests

The test suite includes specific regression tests for bugs found during gameplay:

- **Card stacking bug**: `test_inventory.cpp` verifies duplicate cards don't incorrectly stack
- **Physics edge cases**: `test_rigidbody.cpp` tests extreme positions and masses
- **Seating logic**: `test_poker_table.cpp` validates seat management
- **Community cards memory**: `test_poker_table.cpp` tests that community cards survive multiple hands without use-after-free crashes
- **Shader initialization**: `test_light_bulb.cpp` verifies lighting shader loads correctly and returns by reference

### Test Implementation Notes

- Tests initialize raylib but don't render (tests run headless)
- Physics tests verify behavior without requiring full simulation
- Global variables needed by game code are declared in `tests/test_main.cpp`
- Tests use `nullptr` for physics when physics isn't needed
- Tests clean up dynamically allocated objects to prevent memory leaks

## Development Environment

### IDE Setup (Zed)
A `.clangd` file is configured with:
- Raylib include path
- ODE include path
- Project src path (`-I/Users/michaelslain/Documents/dev/poker/src`)
- C++ language mode (`-x c++`)

### Prerequisites
Raylib and ODE must be installed via Homebrew:
```bash
brew install raylib
brew install ode
```

## Code Architecture

### Object-Oriented Class Hierarchy

**Note**: This shows the C++ inheritance relationships, NOT the file/folder structure. See "Directory Structure" below for file organization.

The game uses C++ inheritance with virtual functions for polymorphism:

```
Object (base class with virtual functions)
├── Interactable
│   ├── Item
│   │   ├── Card (with RigidBody*)
│   │   ├── Chip (with RigidBody*)
│   │   ├── Weapon (abstract base with ammo/shooting)
│   │   │   └── Pistol (6-round revolver)
│   │   └── Substance (abstract base with Consume())
│   │       ├── Weed
│   │       ├── Cocaine
│   │       ├── Molly
│   │       ├── Adrenaline
│   │       ├── Salvia
│   │       ├── Shrooms
│   │       └── Vodka
│   └── PokerTable (poker game manager)
├── Person (abstract base with Inventory)
│   ├── Player (human player with GameCamera)
│   ├── Enemy (AI player)
│   └── Dealer (NPC)
├── Light (base for lighting)
│   └── LightBulb (point light with decorative geometry)
├── Floor (floor geometry)
├── Ceiling (ceiling geometry)
├── Wall (wall geometry)
├── Plane (ground plane with physics collision)
├── RigidBody (physics-enabled objects)
├── ChipStack (chip management)
└── Spawner (creates and manages spawned objects)

Standalone Classes:
GameCamera (first-person camera)
PhysicsWorld (ODE wrapper)
DOM (Document Object Model - manages all objects)
Inventory (dynamic item collection)
Deck (card deck management)
Collider (physics collision component)
Scene (scene data container)
SceneManager (singleton scene switching)
LightingManager (static lighting shader manager)
PsychedelicManager (static psychedelic shader manager)
InsanityManager (player mental state system)
```

**OOP Features**:
- Virtual functions: `Update()`, `Draw()`, `GetType()`
- Constructor/destructor pattern (RAII)
- Member functions instead of free functions
- `std::vector` and `std::array` for collections
- `std::string` for text handling
- `new`/`delete` instead of `malloc`/`free`
- Override keyword for clarity
- Proper encapsulation with public/private members

### Directory Structure

**Note**: This shows the file/folder organization on disk, NOT the C++ class hierarchy. Files are organized by functional category for maintainability.

```
poker/
├── src/                    # All source files organized by category
│   ├── core/              # Core engine systems
│   │   ├── object.hpp/cpp           # Base Object class
│   │   ├── dom.hpp/cpp              # Scene graph manager
│   │   ├── physics.hpp/cpp          # ODE wrapper
│   │   ├── rigidbody.hpp/cpp        # Physics bodies
│   │   ├── collider.hpp/cpp         # Collision components
│   │   ├── scene.hpp/cpp            # Scene data container
│   │   ├── scene_manager.hpp/cpp    # Scene switching
│   │   └── debug.hpp                # Debug utilities
│   │
│   ├── entities/          # Game characters
│   │   ├── person.hpp/cpp           # Abstract base for characters
│   │   ├── player.hpp/cpp           # Human player
│   │   ├── enemy.hpp/cpp            # AI opponent
│   │   └── dealer.hpp/cpp           # Dealer NPC
│   │
│   ├── items/             # Inventory items
│   │   ├── interactable.hpp/cpp     # Base for interactable objects
│   │   ├── item.hpp/cpp             # Pickupable item base
│   │   ├── card.hpp/cpp             # Playing cards
│   │   ├── chip.hpp/cpp             # Poker chips
│   │   ├── chip_stack.hpp/cpp       # Chip stack management
│   │   ├── deck.hpp/cpp             # Card deck
│   │   └── inventory.hpp/cpp        # Inventory system
│   │
│   ├── weapons/           # Weapon classes
│   │   ├── weapon.hpp/cpp           # Abstract weapon base
│   │   └── pistol.hpp/cpp           # 6-round revolver
│   │
│   ├── substances/        # Consumable substances
│   │   ├── substance.hpp/cpp        # Abstract substance base
│   │   ├── weed.hpp/cpp
│   │   ├── cocaine.hpp/cpp
│   │   ├── molly.hpp/cpp
│   │   ├── adrenaline.hpp/cpp
│   │   ├── salvia.hpp/cpp
│   │   ├── shrooms.hpp/cpp
│   │   └── vodka.hpp/cpp
│   │
│   ├── rendering/         # Rendering systems
│   │   ├── camera.hpp/cpp           # First-person camera
│   │   ├── light.hpp/cpp            # Light base class
│   │   ├── light_bulb.hpp/cpp       # Point light with geometry
│   │   ├── lighting_manager.hpp/cpp # Static lighting system
│   │   ├── psychedelic_manager.hpp/cpp # Trip shader system
│   │   ├── inventory_ui.hpp/cpp     # Inventory rendering
│   │   └── render_utils.hpp/cpp     # 3D text utilities
│   │
│   ├── gameplay/          # Game logic
│   │   ├── poker_table.hpp/cpp      # Texas Hold'em implementation
│   │   └── insanity_manager.hpp/cpp # Player mental state
│   │
│   ├── world/             # World geometry
│   │   ├── floor.hpp/cpp            # Floor with collision
│   │   ├── ceiling.hpp/cpp          # Ceiling geometry
│   │   ├── wall.hpp/cpp             # Wall with collision
│   │   └── spawner.hpp/cpp          # Object spawning
│   │
│   └── scenes/            # Scene definitions
│       ├── game_scene.hpp/cpp       # Main game scene
│       └── death_scene.hpp/cpp      # Death/end scene
│
├── tests/              # Catch2 v3.5.0 unit tests
│   ├── catch_amalgamated.hpp/cpp    # Test framework
│   ├── test_main.cpp                # Test entry point
│   └── test_*.cpp                   # Test files (28 total)
│
├── shaders/            # GLSL shaders
│   ├── lighting.vs/fs               # Lighting shader
│   ├── psychedelic.vs/fs            # Trip effect shader
│   └── vignette.vs/fs               # Vignette shader
│
├── main.cpp            # Game entry point
├── Makefile            # Build system
├── CLAUDE.md           # Technical documentation (this file)
└── README.md           # User-facing documentation
```

### Core Components

**Object** (`object.hpp/cpp`):
- Base class with public `position`, `rotation`, `scale` (Vector3)
- Private static `nextID` for unique ID generation
- Private `id` field with `GetID()` accessor
- Public `usesLighting` (bool) - controls whether object uses lighting shader (default: true)
- Virtual functions: `Update(float deltaTime)`, `Draw(Camera3D camera)`, `GetType() const`
- All other components inherit from this
- Uses RAII pattern
- **No child management** - objects added directly to DOM instead
- **Hierarchical type system**: `GetType()` returns concatenated parent types (e.g., "object_interactable_item_card_hearts_ace")

**Person** (`person.hpp/cpp`):
- Inherits from Object
- Abstract base class for Player, Enemy, and Dealer
- Protected members: `inventory` (Inventory), `name` (std::string), `height` (float), `bodyYaw` (float)
- Seating system: `isSeated` (bool), `seatPosition` (Vector3)
- `SitDown(Vector3 seatPos)` and `SitDownFacingPoint(Vector3 seatPos, Vector3 faceTowards)` for seating
- `StandUp()` for standing
- `IsSeated()` to check seating state
- `SetBodyYaw(float)` and `GetBodyYaw()` for rotation control
- Virtual `PromptBet(int currentBet, int callAmount, int minRaise, int maxRaise, int& raiseAmount)` returns 0=fold, 1=call, 2=raise
- Accessors: `GetInventory()`, `GetName()`, `SetName()`, `GetHeight()`, `SetHeight()`
- Sets `usesLighting = false` in constructor (renders pitch black, unaffected by lighting)
- Overrides `Draw()` to render pitch black
- Returns type string in hierarchical format (e.g., "object_person_player")

**Player** (`player.hpp/cpp`):
- Inherits from Person
- First-person controller with WASD movement via `speed` field
- Mouse look with `lookYaw` and `lookPitch` tracking
- Contains `GameCamera` instance
- ODE physics: `dBodyID body` and `dGeomID geom` (capsule)
- Collision categories: `COLLISION_CATEGORY_PLAYER` (1 << 0)
- Inventory: `selectedItemIndex` (-1 = none), `lastHeldItemIndex` for remembering last held
- Betting UI state: `bettingUIActive`, `bettingChoice`, `raiseSliderValue`, `raiseMin`, `raiseMax`, `storedCurrentBet`, `storedCallAmount`
- Card selection UI: `cardSelectionUIActive` (bool), `selectedCardIndices` (vector<int>) for cheating with 3+ cards
- **Insanity system**: Managed by `InsanityManager` instance (public member)
  - Tracks movement, seating state, kills, and psychedelic trips
  - Affects FOV: interpolates from 60° (sane) to 150° (insane)
  - Visualized via `insanityManager.DrawMeter()` - N64-style circular meter in top-right
- Methods: `HandleInteraction()`, `HandleUseItem()`, `GetClosestInteractable()`, `DrawInventoryUI()`, `DrawHeldItem()`, `DrawBettingUI()`, `DrawCardSelectionUI()`, `GetSelectedCards()`, `OnKillPerson()`, `GetInsanity()`
- `HandleUseItem()` calls `item->Use()` on held items, delegates weapon raycast to `Weapon::PerformRaycast()`, handles killing/cleanup
- `DrawHeldItem()` calls `item->DrawHeld()` on held items - works for any item that implements DrawHeld()
- Overrides `SitDown()` and `StandUp()` to handle physics
- Overrides `PromptBet()` for UI-based betting
- Returns type: "player"

**Enemy** (`enemy.hpp/cpp`):
- Inherits from Person
- AI-controlled poker player
- Private members: `thinkingTimer` (float), `thinkingDuration` (float), `isThinking` (bool), `pendingAction` (int)
- Thinking timer system: `thinkingDuration` stores how long to think (2-4 seconds)
- Simple AI logic: Random choice between fold/call/raise
- Height: 1.5x normal (2.7 units) for visual distinction
- Overrides `PromptBet()` with state machine (thinking → decision)
- Overrides `Update()` to handle thinking timer countdown
- Returns type: "enemy"

**Dealer** (`dealer.hpp/cpp`):
- Inherits from Person
- Non-player character positioned at poker table
- Does not participate in poker hands
- Visual presence only (no AI or betting logic)
- Normal height (1.0x)
- Overrides `Update()` and `GetType()`
- Returns type: "dealer"

**GameCamera** (`camera.hpp/cpp`):
- Public members: `camera` (Camera3D), `angle` (Vector2) where x=pitch, y=yaw
- Constructor: `GameCamera(Vector3 pos = {0, 0, 0})`
- `Update(Vector2 mouseDelta)` handles mouse movement
- `AdjustFOV()` handles FOV controls
- `SetTarget(Vector3 target)` positions camera
- `GetCamera()` returns pointer to internal Camera3D

**Interactable** (`interactable.hpp/cpp`):
- Inherits from Object
- Public members: `interactRange` (float), `canInteract` (bool), `onInteract` (std::function callback)
- Constructor: `Interactable(Vector3 pos = {0, 0, 0})`
- Virtual `Interact()` calls onInteract callback if set
- `DrawPrompt(Camera3D)` renders "E" prompt billboard
- `GetType()` returns "interactable"
- Default interact range: 3.0 units

**Item** (`item.hpp/cpp`):
- Inherits from Interactable
- Bridge between Interactable and inventory-compatible objects
- Public `usable` boolean - can this item be used via left-click? (weapons=true, substances=true, cards/chips=false)
- `DrawIcon(Rectangle destRect)` virtual function for UI rendering
- Virtual `Use()` method - called when player left-clicks with item selected (default: does nothing)
- Virtual `DrawHeld(Camera3D camera)` method - renders item in first-person view when held by player (default: does nothing)
- All inventory-compatible objects must inherit from Item

**Weapon** (`weapon.hpp/cpp`):
- Inherits from Item
- Abstract base class for all weapons (Pistol, etc.)
- Protected members: `ammo` (int), `maxAmmo` (int), `rigidBody` (RigidBody*)
- Sets `usable = true` in constructor - weapons can be used via left-click
- Constructor: `Weapon(Vector3 pos, int initialAmmo, int maxAmmoCapacity, PhysicsWorld* physics)`
- Pure virtual methods: `Draw()`, `DrawIcon()`, `DrawHeld()`, `Clone()` - must be implemented by subclasses
- `Update()` syncs with physics body
- `Use()` override - decrements ammo (only if ammo > 0) when player left-clicks
- `Shoot()` legacy method - calls `Use()` for backward compatibility
- `CanShoot()` returns true if ammo > 0
- `PerformRaycast(Vector3 rayStart, Vector3 rayDirection, Person* shooter = nullptr)` - performs cylinder collision raycast to find hit Person
  - Returns `Person*` that was hit, or `nullptr` if no hit
  - `shooter` parameter prevents hitting yourself (filters out the shooter from raycast)
  - Used by Player::HandleUseItem() to determine if weapon hit someone
  - Raycast logic encapsulated in weapon class, not in player
- Accessors: `GetAmmo()`, `GetMaxAmmo()`, `SetAmmo()`, `Reload()`
- `SetAmmo()` clamps to valid range [0, maxAmmo]
- `Reload()` refills to maxAmmo
- Component hierarchy: Weapon → Item → Interactable → Object
- All weapon logic (ammo management, shooting, raycast physics) handled in base class
- Subclasses only implement visual aspects and specify ammo capacity

**Pistol** (`pistol.hpp/cpp`):
- Inherits from Weapon
- 6-round revolver
- Constructor: `Pistol(Vector3 pos, PhysicsWorld* physics)` - calls `Weapon(pos, 6, 6, physics)`
- Implements visual methods: `Draw()`, `DrawIcon()`, `DrawHeld()`, `Clone()`
- `Draw()` renders pistol mesh (grip + barrel) with physics rotation
- `DrawHeld()` renders pistol in first-person view (right side of screen)
- `DrawIcon()` renders 2D icon for inventory UI
- `Clone()` creates new Pistol at specified position
- All ammo/shooting logic inherited from Weapon base class
- Returns type: "object_interactable_item_weapon_pistol"

**Substance** (`substance.hpp/cpp`):
- Inherits from Item
- Abstract base class for all consumable substances (Weed, Cocaine, Molly, etc.)
- Protected members: `rigidBody` (RigidBody*), `color` (Color)
- Sets `usable = true` in constructor - substances can be consumed via left-click
- Constructor: `Substance(Vector3 pos, Color substanceColor, PhysicsWorld* physics)`
- Pure virtual methods: `Consume()`, `GetName()`, `Clone()` - must be implemented by subclasses
- `Update()` syncs with physics body
- `Use()` override final - calls `Consume()` when player left-clicks (automatically removed from inventory after use)
- `Draw()` renders as small colored cube with physics rotation
- `DrawIcon()` renders colored rectangle with substance name
- Component hierarchy: Substance → Item → Interactable → Object
- Each substance implements unique effects in `Consume()` method
- Subclasses specify color, name, and consumption effects

**Card** (`card.hpp/cpp`):
- Inherits from Item
- Component hierarchy: Card → Item → Interactable → Object
- Uses C-style enums: `Suit` (SUIT_HEARTS, SUIT_DIAMONDS, SUIT_CLUBS, SUIT_SPADES)
- Uses C-style enums: `Rank` (RANK_ACE - RANK_KING)
- Contains `RigidBody*` for physics (optional, can be nullptr)
- `RenderTexture2D texture` for card face rendering
- Sets `usesLighting = false` in constructor (manual lighting in Draw method)
- Constructor: `Card(Suit, Rank, Vector3 pos, PhysicsWorld* physics)`
- Destructor: Automatically cleans up texture and rigid body
- `AttachPhysics()` for adding physics after creation
- `Update()` syncs with physics body
- `Draw()` renders as 3D quad with texture (manual lighting calculation)
- `DrawIcon()` renders as 2D icon (flipped)
- Static utility functions: `GetSuitSymbol()`, `GetRankString()`, `GetSuitColor()`

**Chip** (`chip.hpp/cpp`):
- Inherits from Item
- Public members: `value` (int), `color` (Color), `iconTexture` (RenderTexture2D), `iconTextureLoaded` (bool), `rigidBody` (RigidBody*)
- Sets `usesLighting = false` in constructor (renders without lighting)
- Constructor: `Chip(int chipValue, Vector3 pos, PhysicsWorld* physics)`
- Color-coded: WHITE(1), RED(5), BLUE(10), GREEN(25), BLACK(100)
- Destructor: Automatically cleans up texture and rigid body
- `Update()` syncs with physics body
- `Draw()` renders as 3D cylinder (handles chips with or without physics)
- `DrawIcon()` renders as 2D icon
- `GetType()` returns hierarchical type (e.g., "object_interactable_item_chip_5")
- Static function: `GetColorFromValue(int value)`

**Inventory** (`inventory.hpp/cpp`):
- Uses `std::vector<ItemStack>` for dynamic storage
- `ItemStack` struct: `Item* item`, `int count`, `std::string typeString`
- `AddItem(Item*)` returns bool, adds or increments stack, automatically sorts after adding
- `RemoveItem(int stackIndex)` returns bool, decrements or removes stack
- `Cleanup()` cleans up all items
- `Sort()` automatically sorts inventory by category and value:
  - **Category order**: Weapons → Cards → Chips
  - **Cards**: Sorted by rank (2-King, then Ace)
  - **Chips**: Sorted by value (1, 5, 10, 25, 100)
  - Uses `std::stable_sort` with null-safe comparisons
- Accessors: `GetStackCount()`, `GetStack(int index)`, `GetStacks()`
- Automatic cleanup in destructor

**InventoryUI** (`inventory_ui.hpp/cpp`):
- `InventoryUI_Draw(Inventory*, int selectedIndex)` function
- Renders items as 60x60 icons at top-left
- White outline for selected item
- Displays stack count if > 1
- Delegates to `Item::DrawIcon()` polymorphically

**Deck** (`deck.hpp/cpp`):
- Inherits from Object
- Private members: `cards` (vector<Card*>) for stack, `allCards` (vector<Card*>) for cleanup
- Constructor: `Deck(Vector3 pos = {0, 0, 0})` creates all 52 cards (no physics)
- `Shuffle()` uses Fisher-Yates algorithm on stack
- `DrawCard()` pops from top of stack (back of vector)
- `Peek()` returns top card without removing
- `Reset()` pushes all cards back onto stack
- `Cleanup()` and destructor clean up all cards
- Accessors: `GetCount()`, `IsEmpty()`
- Overrides: `Update()`, `Draw()`, `GetType()`

**PokerTable** (`poker_table.hpp/cpp`):
- Inherits from Interactable
- Private visual: `size` (Vector3), `color` (Color), `geom` (dGeomID), `physics` (PhysicsWorld*)
- Game objects: `dealer` (Dealer*), `deck` (Deck*), `potStack` (ChipStack*), `communityCards` (vector<Card*>)
- Seating: `seats` (array<Seat, MAX_SEATS>), `statusList` (array<int, MAX_SEATS>), `hasRaised` (array<bool, MAX_SEATS>)
- Hole cards tracking: `dealtHoleCards` (array<vector<Card*>, MAX_SEATS>) for end-of-hand cleanup
- Game state: `smallBlindSeat`, `bigBlindSeat`, `currentPlayerSeat`, `currentBet`, `potValue`, `handActive`, `bettingActive`, `showdownActive`
- Logging: `lastLoggedPlayerSeat` to prevent duplicate logs
- Helper methods: `CountChips()`, `TakeChips()`, `GiveChips()`, `CalculateChipCombination()`, `NextOccupiedSeat()`, `NextActiveSeat()`, `GetOccupiedSeatCount()`, `IsBettingRoundComplete()`, `ProcessBetting()`, `EvaluateHand()`, `CompareHands()`
- Game flow: `StartHand()`, `DealHoleCards()`, `PostBlinds()`, `StartBettingRound()`, `DealFlop()`, `DealTurn()`, `DealRiver()`, `Showdown()`, `EndHand()`
- Constructor: `PokerTable(Vector3 pos, Vector3 size, Color color, PhysicsWorld*)`
- Seating: `FindClosestOpenSeat()`, `SeatPerson()`, `UnseatPerson()`, `FindSeatIndex()`
- Accessor: `GetGeom()` returns dGeomID
- Overrides: `Update()`, `Draw()`, `Interact()`, `GetType()`
- Defines: `MAX_SEATS` (8), `SMALL_BLIND_AMOUNT` (5), `BIG_BLIND_AMOUNT` (10)
- Collision: `COLLISION_CATEGORY_TABLE` (1 << 2)

**Poker Game Logic**:
- **Seat management**: `Seat` struct with `position` (Vector3), `occupant` (Person*), `isOccupied` (bool)
- **Status tracking**: `statusList` array (-1 = folded, >= 0 = current bet), `hasRaised` array
- **Dealer button**: Rotates with `NextOccupiedSeat()` helper
- **Blinds**: Posted in `PostBlinds()` via `TakeChips()` helper (SB=5, BB=10)
- **Hole cards**: Deal 2 cards per player directly into inventories, tracked in `dealtHoleCards` for cleanup
- **Community cards**: Stored in `communityCards` vector, added to DOM for rendering
- **Betting**: `ProcessBetting(dt)` called each frame handles all betting logic
- **Value-based chips**: `CountChips()`, `TakeChips()`, `GiveChips()` helpers use inventory system
- **Chip combinations**: `CalculateChipCombination()` optimizes denominations (100s, 25s, 10s, 5s, 1s)
- **Hand flow**: `StartHand()` → `PostBlinds()` → `DealHoleCards()` → betting → `DealFlop()` → betting → `DealTurn()` → betting → `DealRiver()` → betting → `Showdown()` → `EndHand()`
- **Hand evaluation**: `EvaluateHand()` returns `HandEvaluation` struct with `HandRank` enum (HIGH_CARD to ROYAL_FLUSH) and `rankValues` for tie-breaking
- **Winner determination**: `CompareHands()` compares two `HandEvaluation` structs, returns -1/0/1
- **Showdown**: Player with 3+ cards uses card selection UI to choose 2 cards for evaluation
- **Cleanup**: `EndHand()` removes hole cards from inventories (from `dealtHoleCards` tracking), deletes them, clears community cards, starts next hand

**Key Implementation Details**:
- **Hole cards tracking**: `dealtHoleCards` array tracks which cards were dealt this hand for proper cleanup
- **DOM-based rendering**: Objects added to DOM for rendering (dealer, potStack, community cards)
- **Deck management**: Deck is NOT added to DOM (remains hidden), but deck OWNS all card objects
- **Community cards**: Added to DOM when dealt (`DealFlop/Turn/River`), removed from DOM in `EndHand()`, but NOT deleted (deck owns them)
- **Card memory safety**: Community cards are removed from DOM but never deleted - the deck reuses them via `Reset()`
- **Mid-hand joining**: Players marked with `statusList[seat] = -1` (folded) if joining during active hand
- **Person::PromptBet()**: Takes currentBet, callAmount, minRaise, maxRaise, raiseAmount reference; returns 0=fold, 1=call, 2=raise
- **Enemy thinking**: `isThinking` flag with `thinkingTimer` countdown before returning action
- **Player betting UI**: `bettingUIActive` flag, displays fold/call/raise options with slider
- **Player card selection**: `cardSelectionUIActive` flag when showdown with 3+ cards, selects 2 cards via `selectedCardIndices`
- **Chip affordability**: Checks `CountChips(person)` total value via inventory iteration
- **Automatic progression**: Hands start automatically when 2+ players seated via `GetOccupiedSeatCount()`
- **Logging**: `GAME_LOG` macro for poker table events, `lastLoggedPlayerSeat` prevents duplicates

### Physics System

**PhysicsWorld** (`physics.hpp/cpp`):
- Wrapper around ODE (Open Dynamics Engine)
- Contains `dWorldID`, `dSpaceID`, `dJointGroupID`
- Static global instance pointer (similar to DOM)
- Constructor initializes ODE with gravity (0, -9.81, 0)
- Destructor cleans up ODE resources
- `Step(float deltaTime)` advances physics simulation
- Static `NearCallback()` for collision handling
- Handles contact joints and surface properties
- **Global instance management**:
  - `SetGlobal(PhysicsWorld*)` - Set global physics instance
  - `GetGlobal()` - Get global physics instance (returns nullptr if not set)
  - Allows objects like Spawner to access physics without parameter passing
  - Example: `PhysicsWorld::SetGlobal(&physics);` in scene initialization

**RigidBody** (`rigidbody.hpp/cpp`):
- Inherits from Object
- Contains ODE `dBodyID` and `dGeomID`
- Reference to `PhysicsWorld*`
- `InitBox(PhysicsWorld*, Vector3 pos, Vector3 size, float mass)`
- `InitSphere(PhysicsWorld*, Vector3 pos, float radius, float mass)`
- `Update(float deltaTime)` syncs Object position with physics body
- `GetRotationMatrix()` converts ODE rotation to raylib Matrix
- Destructor destroys ODE body and geometry
- Collision categories for items vs. player

**Plane** (`plane.hpp/cpp`):
- Inherits from Object
- Represents ground plane with physics collision
- Contains ODE `dGeomID` (static geometry, no body)
- Constructor: `Plane(Vector3 position, Vector2 size, Color color, PhysicsWorld*)`
- `Draw()` renders plane rectangle
- Destructor destroys ODE geometry
- Used for ground and static surfaces

### DOM (Document Object Model)

**DOM** (`dom.hpp/cpp`):
- Uses `std::vector<Object*>` for dynamic storage
- Static global instance pointer
- `AddObject(Object*)` adds to vector
- `RemoveObject(Object*)` erases from vector
- `Cleanup()` clears vector (doesn't delete objects)
- Accessors: `GetCount()`, `GetObject(int)`, `GetObjects()`
- Static functions: `SetGlobal(DOM*)`, `GetGlobal()`
- All scene objects tracked here for updates and rendering

**DOM Usage**:
- Player added to DOM at initialization
- Ground plane added to DOM
- Spawners create objects and add to DOM
- On item pickup: add to inventory, remove from DOM, deactivate
- Iterate DOM for rendering and updates

### Spawner System

**Spawner** (`spawner.hpp/cpp`):
- Inherits from Object
- Self-contained object spawning system
- Private members: `radius` (float), `templateObject` (Object*), `count` (int), `hasSpawned` (bool)
- Constructor: `Spawner(Vector3 pos, float radius, Object* templateObject, int spawnCount)`
- **Takes ownership** of template object and deletes it in destructor
- Spawns automatically on construction (no manual method calls needed)
- Uses global DOM and PhysicsWorld (no parameter passing required)
- Spawns `count` copies of the template object randomly within `radius`
- Supports spawning: Cards, Chips, Pistols (extensible to any Object type)
- `PerformSpawn()` private method handles spawning logic
- Example usage:
  ```cpp
  // Spawn 3 Ace of Spades cards in a 2-unit radius
  new Spawner({0, 2, 0}, 2.0f, new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr), 3);

  // Spawn 5 chips worth 100 in a 1.5-unit radius
  new Spawner({-5, 2, -3}, 1.5f, new Chip(100, {0,0,0}, nullptr), 5);
  ```

### Lighting System

**LightingManager** (`lighting_manager.hpp/cpp`):
- Static utility class for managing the global lighting shader system
- No instances created - all methods and members are static
- Static members: `lightingShader` (Shader), `shaderInitialized` (bool), `lightsCount` (int)
- **Initialization**: `InitLightingSystem()` - loads shaders/lighting.vs and shaders/lighting.fs
- **Cleanup**: `CleanupLightingSystem()` - unloads shader
- **Camera updates**: `UpdateCameraPosition(Vector3)` - updates view position uniform
- **Light management**:
  - `CreateLight(int type, Vector3 position, Vector3 target, Color color)` - creates RaylibLight struct
  - `UpdateLightValues(RaylibLight light)` - sends light data to shader
- **Constants**: `MAX_LIGHTS` (4), `LightType` enum (LIGHT_DIRECTIONAL=0, LIGHT_POINT=1)
- **RaylibLight struct**: Contains type, enabled, position, target, color, and shader location IDs
- Must call `InitLightingSystem()` before creating any lights
- Must call `CleanupLightingSystem()` before closing window

**Light** (`light.hpp/cpp`):
- Inherits from Object
- Base class for all light objects in the scene (LightBulb, etc.)
- Sets `usesLighting = false` in constructor (light sources don't get lit)
- Virtual `UpdateLight()` method - default implementation is empty, override in derived classes
- Returns hierarchical type: "object_light"
- Acts as marker class for polymorphic light management

**LightBulb** (`light_bulb.hpp/cpp`):
- Inherits from Light
- Hanging lantern-style light bulb with visible geometry
- Private members: `raylibLightPtr` (void* to RaylibLight), `color` (Color)
- Constructor: `LightBulb(Vector3 position, Color lightColor)`
  - Creates point light via `LightingManager::CreateLight()`
  - Stores light struct as opaque pointer for encapsulation
- `UpdateLight()` - syncs RaylibLight position with Object position and updates shader
- `Draw()` - renders decorative bulb geometry:
  - Hanging chain/wire from ceiling
  - Lantern top fixture (metal cap)
  - Glass housing (semi-transparent cylinder)
  - Bulb shape (teardrop of spheres)
  - Metal screw base
  - Layered glow halos for painterly effect
- Destructor cleans up heap-allocated RaylibLight
- Returns type: "object_light_light_bulb"
- Main loop must call `UpdateLight()` each frame to sync position

### Psychedelic Effect System

**PsychedelicManager** (`psychedelic_manager.hpp/cpp`):
- Static utility class for managing psychedelic visual effects (shrooms trip)
- No instances created - all methods and members are static
- Static members: `psychedelicShader` (Shader), `shaderInitialized` (bool), `tripStartTime` (float), `baseIntensity` (float), `isTripping` (bool)
- **Initialization**: `InitPsychedelicSystem()` - loads shaders/psychedelic.vs and shaders/psychedelic.fs
- **Cleanup**: `CleanupPsychedelicSystem()` - unloads shader
- **Trip control**:
  - `StartTrip(float intensity)` - begins psychedelic trip with specified intensity (0.0-1.0)
  - `StopTrip()` - immediately ends trip
  - `Update(float deltaTime)` - advances trip time and auto-stops after 5 minutes
- **Trip stages** (automatic progression):
  - **Come up** (0-60s): Intensity ramps from 0 → 1 with smooth curve
  - **Peak** (60-180s): Full intensity with oscillating waves (0.7-1.0)
  - **Come down** (180-300s): Intensity gradually fades from 1 → 0
- **Accessors**: `IsTripping()`, `GetCurrentIntensity()`, `GetTripTime()`, `GetPsychedelicShader()`
- **Constants**: `TRIP_DURATION` (300 seconds = 5 minutes)
- Must call `InitPsychedelicSystem()` after `InitWindow()` and before game loop
- Must call `CleanupPsychedelicSystem()` before `CloseWindow()`

**Psychedelic Shader** (`shaders/psychedelic.vs` & `shaders/psychedelic.fs`):
- Post-processing shader applied to entire 3D scene when tripping
- **Uniforms**: `time` (float), `intensity` (float)
- **Visual effects** (all intensity-modulated):
  - **Breathing/Morphing**: Sine wave distortions that pulse with time
  - **Drifting/Warping**: Multi-octave fractal Brownian motion noise
  - **Geometric patterns**: Spirals, tunnels, lattices, cobweb patterns
  - **Color shifting**: HSV manipulation with hue rotation and saturation boost
  - **Pattern overlays**: Dynamic geometric overlays during peak phase
  - **Edge glow**: Radial glow effects during peak phase
- **Effect progression**: Intensity and complexity increase through come-up, peak at stage 1-2, fade during come-down
- Uses simplex noise functions for organic warping
- Polar coordinates for spiral/tunnel effects
- RGB ↔ HSV conversion for color manipulation

**Shrooms Integration** (`shrooms.hpp/cpp`):
- Inherits from Substance
- Purple color: `{150, 100, 200, 255}`
- `Consume()` calls `PsychedelicManager::StartTrip(1.0f)` for full-intensity trip
- Trip lasts 5 minutes with automatic progression through stages
- Player experiences visual distortions, color shifts, and geometric patterns

**Main Loop Integration** (`main.cpp`):
- Render-to-texture pipeline for post-processing
- 3D scene rendered to `RenderTexture2D`
- If tripping: psychedelic shader applied with time/intensity uniforms
- Shader-processed texture drawn to screen
- UI elements rendered on top (unaffected by shader)
- Proper cleanup: `UnloadRenderTexture()` before `CloseWindow()`

### Insanity System

**InsanityManager** (`insanity_manager.hpp/cpp`):
- Manages player mental state based on behavior and trauma
- Private members: `insanity`, `minInsanity`, `minInsanityDecayTimer`, `timeSinceLastMove`, `lastPosition`
- **Configuration constants** (static constexpr):
  - `INSANITY_DECREASE_RATE = 0.3f` - When moving
  - `INSANITY_INCREASE_SEATED = 0.01f` - When seated still
  - `INSANITY_INCREASE_STANDING = 0.02f` - When standing still
  - `MIN_INSANITY_DECAY_RATE = 0.05f` - Floor decay rate (after hold period)
  - `MIN_INSANITY_HOLD_TIME = 30.0f` - Seconds before floor starts decaying
  - `KILL_INSANITY_INCREASE = 0.2f` - Per kill trauma
- **Update system**: `Update(deltaTime, currentPosition, isSeated, isTripping, tripIntensity)`
  - Movement detection with 0.01 unit threshold
  - Different rates for seated vs standing
  - Psychedelic trip integration: `insanity = tripIntensity + minInsanity`
  - Clamps to [minInsanity, 1.0] when not tripping
- **Kill trauma system**: `OnKill()`
  - Increases `minInsanity` by 0.2 per kill (stacks)
  - Sets 30-second hold timer before decay
  - Creates psychological "floor" - insanity can't drop below this
  - After timer: floor decays at 0.05/sec (20 seconds to decay 0.2)
- **Psychedelic integration**:
  - While tripping: insanity = trip intensity + minimum floor
  - Kills compound trip intensity (traumatized trips are more intense)
  - Example: 1 kill (0.2 floor) + 0.5 trip = 0.7 total insanity
- **Visual feedback**: `DrawMeter()` - N64-style circular meter in top-right
  - Yellow → Orange → Red color gradient
  - Fills clockwise from top as insanity increases
- **Player integration**:
  - Player owns public `InsanityManager insanityManager` member
  - `OnKillPerson()` delegates to manager
  - FOV warping driven by `insanityManager.GetInsanity()`

### Death Scene System

**DeathScene** (`src/scenes/death_scene.hpp/cpp`):
- **Purpose**: End-game scene displayed when player dies from insanity
- **Architecture**: Factory function pattern (`CreateDeathScene(PhysicsWorld*)`)
- **Components**:
  - `DeathSceneObject`: Simple Object subclass that renders "THE END" text
  - Renders in 2D (ignores camera parameter)
  - No physics or game logic
- **Integration**:
  - Registered in SceneManager as "death" scene factory
  - Main loop checks `player->IsDead()` after rendering each frame
  - On death: cleans up game scene, switches to death scene via SceneManager
  - Player pointer nullified before cleanup to prevent use-after-free
- **Rendering**:
  - White text centered on black background
  - Font size: 60px
  - Text: "THE END"
  - Draw() method uses raylib 2D functions (DrawText)
- **Type system**: Returns `"object_death_scene_renderer"`
- **Memory management**: Scene owns and deletes DeathSceneObject
- **Testing**: Comprehensive tests in `test_death_scene.cpp` (29 assertions)

**Death Transition Flow** (in `main.cpp`):
1. Each frame: Check `player->IsDead()` after rendering
2. On death:
   - Log death event
   - Nullify `player` and `closestInteractable` pointers
   - Delete all DOM objects (cleanup game scene)
   - Call `dom.Cleanup()` to clear vector
   - Create death scene via `sceneManager->CreateScene("death", &physics)`
   - Add death scene objects to DOM
   - `continue` to next frame (skip rest of loop)
3. Subsequent frames render death scene (no player check needed)

### Rendering Utilities

**render_utils** (`render_utils.hpp/cpp`):
- `DrawTextBillboard()`: Renders text facing camera
- `DrawText3D()`: Renders text in 3D space
- Shared utilities for 3D text rendering

### Important Rendering Notes

**RenderTexture2D Usage**:
- Cards and chips use `BeginTextureMode/EndTextureMode`
- Textures created once in constructor
- 2D UI: negative height in source rectangle to flip
- 3D rendering: normal UV coordinates work correctly
- Cleanup in destructor unloads textures

**3D Physics Object Rendering**:
- Cards and chips use `RigidBody::GetRotationMatrix()`
- `rlPushMatrix()` / `rlPopMatrix()` for transforms
- `rlMultMatrixf()` applies rotation matrix
- Automatically synced with physics

**Lighting System Architecture**:
- **Centralized management**: `LightingManager` static class manages global shader and lights
- **Object-based lighting control**: Objects use `usesLighting` boolean (default: true)
- **Light hierarchy**: `Light` (base) → `LightBulb` (derived) for polymorphic light management
- **Main rendering loop** separates lit and unlit objects:
  1. Lit objects: rendered inside `BeginShaderMode(LightingManager::GetLightingShader())` / `EndShaderMode()`
  2. Unlit objects: rendered normally after shader mode
- **Objects that set `usesLighting = false`**:
  - Cards (use manual lighting calculation in Draw)
  - Chips (render without lighting)
  - Persons (render pitch black)
  - Light objects (Light, LightBulb - shouldn't be affected by lighting)
- **Type checking**: Use `if (!obj->usesLighting)` instead of type string checks
- **Light synchronization**: Call `UpdateLight()` on all Light-derived objects each frame
- **Cleanup order**: Must call `LightingManager::CleanupLightingSystem()` BEFORE `CloseWindow()`
- **No rlights.h dependency**: All lighting utilities moved to LightingManager class

### Memory Management

**C++ RAII Pattern**:
- Constructors initialize resources
- Destructors clean up resources automatically
- `new`/`delete` for dynamic allocation
- Smart ownership model
- Must call `delete` on dynamically allocated objects
- DOM tracks objects but doesn't own them

**Object Lifecycle**:
1. Create with `new` (or let Spawner do it)
2. Constructor initializes (e.g., `Card`, `Chip`)
3. Add to DOM with `dom.AddObject()`
4. Object exists in world with physics
5. `Update()` called each frame
6. On pickup: add to inventory, `dom.RemoveObject()`
7. Cleanup: `delete` dynamically allocated objects
8. Destructors handle resource cleanup

**Cleanup Pattern**:
```cpp
// Main cleanup loop
for (int i = 0; i < dom.GetCount(); i++) {
    Object* obj = dom.GetObject(i);
    delete obj;  // Destructor handles cleanup
}
dom.Cleanup();  // Clear the DOM vector
```

### Controls
- **WASD**: Move around
- **Mouse**: Look around (reduced sensitivity: 0.001f)
- **U**: Toggle cursor lock/unlock
- **E**: Interact with closest object (crosshair-based)
- **X**: Select/deselect item in inventory
- **Left/Right Arrow**: Navigate inventory selection
- **[ ]**: Increase/decrease FOV
- **ESC**: Close window

### PokerTable Usage Example
```c
// Initialize physics and DOM
PhysicsWorld physics;
Physics_Init(&physics);

DOM dom;
DOM_Init(&dom, 50);

// Create player
Player player;
Player_Init(&player, (Vector3){0, 0, -5});
DOM_AddObject(&dom, (Object*)&player);

// Create poker table
PokerTable* table = malloc(sizeof(PokerTable));
Vector3 tableSize = { 4.0f, 0.2f, 2.5f };  // Wide, thin, deep
PokerTable_Init(table, (Vector3){0, 1.0f, 0}, tableSize, BROWN);
DOM_AddObject(&dom, (Object*)table);

// In game loop - find closest interactable
Interactable* closest = NULL;
float closestDist = FLT_MAX;

for (int i = 0; i < dom.count; i++) {
    const char* type = dom.objects[i]->GetType(dom.objects[i]);

    if (strcmp(type, "poker_table") == 0) {
        Interactable* interactable = (Interactable*)dom.objects[i];
        float dist = Vector3Distance(player.base.position, interactable->base.position);

        if (dist < interactable->interactRange && dist < closestDist) {
            closest = interactable;
            closestDist = dist;
        }
    }
}

// Handle interaction
if (closest && IsKeyPressed(KEY_E)) {
    if (strcmp(closest->base.GetType((Object*)closest), "poker_table") == 0) {
        PokerTable_InteractWithPlayer((PokerTable*)closest, &player);
    }
}

// Cleanup
PokerTable_Cleanup(table);
free(table);
```

### Main Game Loop Pattern
```cpp
// Initialization
PhysicsWorld physics;  // Constructor initializes ODE
DOM dom;               // Constructor initializes vector
DOM::SetGlobal(&dom);

// Create player
Player* player = new Player({0, 0, 0}, &physics);
dom.AddObject(player);

// Create ground
Plane* ground = new Plane({0, 0, 0}, {50, 50}, LIGHTGRAY, &physics);
dom.AddObject(ground);

// Create poker table
PokerTable* table = new PokerTable({5, 1, 0}, {4, 0.2, 2.5}, BROWN, &physics);
dom.AddObject(table);

// Set global physics for spawners
PhysicsWorld::SetGlobal(&physics);

// Create spawners - they spawn automatically on construction
Spawner* cardSpawner = new Spawner({0, 2, 0}, 2.0f, new Card(SUIT_SPADES, RANK_ACE, {0,0,0}, nullptr), 3);
Spawner* chipSpawner = new Spawner({-5, 2, -3}, 1.5f, new Chip(5, {0,0,0}, nullptr), 10);
dom.AddObject(cardSpawner);
dom.AddObject(chipSpawner);

SetTargetFPS(60);

// Game loop
while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();

    // Update
    player->Update(deltaTime);
    physics.Step(deltaTime);

    for (int i = 0; i < dom.GetCount(); i++) {
        dom.GetObject(i)->Update(deltaTime);
    }

    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);

    Camera3D* cam = player->GetCamera();
    BeginMode3D(*cam);
        for (int i = 0; i < dom.GetCount(); i++) {
            dom.GetObject(i)->Draw(*cam);
        }
    EndMode3D();

    player->DrawInventoryUI();
    DrawFPS(10, screenHeight - 30);
    EndDrawing();
}

// Cleanup
for (int i = 0; i < dom.GetCount(); i++) {
    Object* obj = dom.GetObject(i);
    delete obj;  // Calls destructor
}
dom.Cleanup();  // Clear the DOM vector

// PhysicsWorld destructor called automatically
```

### Type System
Objects use virtual `GetType() const` for runtime type identification:
- **Hierarchical format**: Returns concatenated parent types (e.g., "object_interactable_item_card_hearts_ace")
- **Type checking**: Use `type.find("keyword") != std::string::npos` for substring matching
  - Example: `type.find("card") != std::string::npos` matches all card types
  - Example: `type.find("person") != std::string::npos` matches Player, Enemy, Dealer
  - Example: `type.find("chip") != std::string::npos` matches all chip values
- **Avoid exact matching**: Don't use `type == "player"` - won't work with hierarchical types
- Polymorphic - overridden in each derived class

### Common Patterns

**Creating Objects**:
```cpp
// Direct allocation
Card* card = new Card(SUIT_SPADES, RANK_ACE, {0, 2, 0}, &physics);
dom.AddObject(card);

// Or use Spawner (recommended) - spawns automatically on construction
Spawner* spawner = new Spawner({0, 2, 0}, 2.0f, new Card(SUIT_HEARTS, RANK_KING, {0,0,0}, nullptr), 5);
dom.AddObject(spawner);  // Add spawner to DOM for tracking
```

**Polymorphic Updates**:
```cpp
for (int i = 0; i < dom.GetCount(); i++) {
    dom.GetObject(i)->Update(deltaTime);  // Calls correct virtual function
}
```

**Item Pickup**:
```cpp
Item* item = static_cast<Item*>(interactable);
player->GetInventory()->AddItem(item);
item->isActive = false;
dom.RemoveObject(item);
```

**Collision Detection**:
```cpp
// Player checks collision with table
dContactGeom contacts[4];
int numContacts = dCollide(playerGeom, tableGeom, 4, contacts, sizeof(dContactGeom));
if (numContacts > 0) {
    // Handle collision
}
```

### Common Pitfalls

1. **Virtual Functions**: Always mark overrides with `override` keyword for safety

2. **Memory Management**: Delete dynamically allocated objects; destructors handle cleanup

3. **Physics Order**: Call `physics.Step()` before `Update()` calls

4. **DOM Ownership**: DOM doesn't own objects - must delete separately

5. **Texture Cleanup**: Destructors automatically unload RenderTexture2D

6. **Collision Categories**: Set proper ODE collision bits for player/item/table

7. **Enum Syntax**: Use C-style enums (SUIT_SPADES, not Suit::SPADES)

8. **Null Checks**: Check `rigidBody != nullptr` before accessing

9. **Spawner Auto-spawn**: Spawners spawn objects automatically on construction - no manual method calls needed

10. **Constructor Initialization**: Use member initializer lists for efficiency

11. **Card Ownership**: NEVER delete cards from community cards or inventories - the Deck owns them and reuses them

12. **DOM Architecture**: Objects should NEVER render other objects directly - add them to DOM instead

13. **Rendering Without Lighting**: Cards, chips, light sources, and persons skip the lighting shader for proper appearance

14. **Shader Return Types**: Always return shader references (`Shader&`) not copies to avoid OpenGL resource duplication

15. **Lighting Initialization Order**: Call `LightingManager::InitLightingSystem()` AFTER `InitWindow()` but BEFORE creating any Light objects

16. **Lighting Cleanup Order**: Call `LightingManager::CleanupLightingSystem()` BEFORE `CloseWindow()` to avoid shader cleanup errors

17. **Light Position Updates**: Remember to call `UpdateLight()` on Light-derived objects each frame to sync shader uniforms
