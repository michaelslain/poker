## Project Overview
A first-person poker game built with C and raylib using a component-based architecture with inventory system, DOM-based object management, and ODE physics engine integration.

## Build System
The project uses a Makefile with macOS-specific raylib paths:
- Raylib include path: `/opt/homebrew/opt/raylib/include`
- Raylib library path: `/opt/homebrew/opt/raylib/lib`
- Compiler flags: `-Wall -Wextra -std=c99 -Iinclude`
- Target executable: `game`
- Links against: `-lraylib -lode`

### Build Commands
```bash
make           # Compile the project
make run       # Build and run the game
make clean     # Remove build artifacts (*.o and game executable)
```

## Development Environment

### IDE Setup (Zed)
A `.clangd` file is configured with:
- Raylib include path
- Project include path (`-I/Users/michaelslain/Documents/dev/poker/include`)
- C language mode (`-x c`)

### Prerequisites
Raylib and ODE must be installed via Homebrew:
```bash
brew install raylib
brew install ode
```

## Code Architecture

### Component Hierarchy
The game uses an inheritance-style component system through struct composition:

```
Object (base)
├── Interactable
│   └── Item
│       ├── Card (with RigidBody)
│       └── Chip (with RigidBody)
├── Player (contains GameCamera and Inventory)
├── GameCamera
├── Plane (ground plane with physics collision)
├── RigidBody (physics-enabled objects)
└── Spawner (creates and manages spawned objects)
```

**Inheritance Pattern**: Each "child" struct contains the "parent" struct as its first member, allowing safe casting.

### Directory Structure
```
poker/
├── include/          # Header files
│   ├── object.h
│   ├── camera.h
│   ├── player.h
│   ├── interactable.h
│   ├── item.h
│   ├── card.h
│   ├── chip.h
│   ├── deck.h
│   ├── poker_table.h
│   ├── inventory.h
│   ├── inventory_ui.h
│   ├── render_utils.h
│   ├── dom.h
│   ├── spawner.h
│   ├── physics.h
│   ├── rigidbody.h
│   └── plane.h
├── src/              # Implementation files
│   ├── object.c
│   ├── camera.c
│   ├── player.c
│   ├── interactable.c
│   ├── item.c
│   ├── card.c
│   ├── chip.c
│   ├── deck.c
│   ├── poker_table.c
│   ├── inventory.c
│   ├── inventory_ui.c
│   ├── render_utils.c
│   ├── spawner.c
│   ├── physics.c
│   ├── rigidbody.c
│   └── plane.c
└── main.c            # Game entry point with DOM
```

### Core Components

**Object** (`object.h/c`):
- Base component with position, rotation, scale
- Has active flag for lifecycle management
- Type system: `GetType()` virtual function returns type string
- All other components inherit from this

**Player** (`player.h/c`):
- First-person controller with WASD movement
- Mouse look (inverted: moving mouse left pans left, moving mouse up pans up)
- Contains its own `GameCamera` instance
- Contains an `Inventory` instance
- FOV control with `[` and `]` keys
- Pickup behavior: adds items to inventory, removes from DOM
- No jumping implemented yet
- `Player_GetInventory()` accessor for inventory
- `Player_DrawInventoryUI()` renders inventory icons
- `Player_GetType()` returns "player"

**GameCamera** (`camera.h/c`):
- Wraps raylib's Camera3D
- Tracks pitch/yaw angles
- `GameCamera_AdjustFOV()` handles FOV controls
- Eye-level offset: 1.7 units above player position

**Interactable** (`interactable.h/c`):
- Objects player can interact with (press E)
- 3-unit interaction range
- Closest interactable is highlighted
- Callback system: `InteractCallback` for custom behaviors
- Default behavior: deactivate (delete) the object

**Item** (`item.h/c`):
- Inherits from Interactable
- Bridge between Interactable and inventory-compatible objects
- Default interact behavior: deactivates item (for pickup)
- `Item_DrawIcon(Rectangle destRect)`: Virtual function for UI rendering
- All inventory-compatible objects must inherit from Item

**Card** (`card.h/c`):
- Inherits from Item
- Component hierarchy: Card → Item → Interactable → Object
- Has `Suit` (SUIT_HEARTS, SUIT_DIAMONDS, SUIT_CLUBS, SUIT_SPADES) and `Rank` (RANK_ACE-RANK_KING)
- Contains a `RigidBody` for physics simulation (optional)
- **Texture-based rendering**: Each card generates a RenderTexture2D once during init
- Displays rank (large) and suit name (smaller) on white background
- Colors: Red for hearts/diamonds, black for clubs/spades
- `Card_Init()`: Initialize card with optional physics (pass NULL to skip physics)
- `Card_AttachPhysics()`: Attach physics to a card after creation (useful for deck cards)
- `Card_DrawIcon()`: Renders texture as 2D icon with negative height to flip correctly
- `Card_Update()`: Syncs physics body with object position (only if physics attached)
- `Card_Draw()`: Renders card as 3D quad with proper orientation using rotation matrix
- `Card_Cleanup()`: Unloads texture and cleans up rigid body
- `Card_GetType()`: Returns type string like "card_ace_spades"

**Chip** (`chip.h/c`):
- Inherits from Item
- Component hierarchy: Chip → Item → Interactable → Object
- Has integer `value` field (1, 5, 10, 25, 100, etc.)
- Contains a `RigidBody` for physics simulation
- Color-coded by value: WHITE(1), RED(5), BLUE(10), GREEN(25), BLACK(100)
- **Texture-based rendering**: Each chip generates a RenderTexture2D once during init
- Displays value as text on colored circle
- `Chip_DrawIcon()`: Renders texture as 2D icon with negative height to flip correctly
- `Chip_Update()`: Syncs physics body with object position
- `Chip_Draw()`: Renders chip as 3D cylinder with proper orientation
- `Chip_Cleanup()`: Unloads texture and cleans up rigid body
- `Chip_GetColorFromValue()`: Returns color based on chip value
- `Chip_GetType()`: Returns type string like "chip_5" or "chip_100"

**Inventory** (`inventory.h/c`):
- Dynamic array of Item pointers
- Auto-expands capacity when full (doubles capacity)
- `Inventory_Init(int initialCapacity)`: Initialize with starting capacity
- `Inventory_AddItem(Item* item)`: Add item to inventory
- `Inventory_Cleanup()`: Free all items and array memory
- Tracks `count` and `capacity` separately

**InventoryUI** (`inventory_ui.h/c`):
- Separate module for rendering inventory contents
- `InventoryUI_Draw(Inventory* inventory)`: Draws all items as 60x60 icons
- Renders in horizontal row at top-left (10, 10)
- Delegates to `Item_DrawIcon()` for each item's rendering

**Deck** (`deck.h/c`):
- NOT an Object - standalone data structure
- NOT added to DOM - manages card allocation separately
- Contains array of 52 Card pointers (all suits and ranks)
- `Deck_Init()`: Allocates and initializes all 52 cards without physics
- `Deck_Shuffle()`: Randomly shuffles the deck using Fisher-Yates algorithm
- `Deck_Draw()`: Returns top card and removes it from deck (returns NULL if empty)
- `Deck_Peek()`: Returns top card without removing it (returns NULL if empty)
- `Deck_Reset()`: Resets count to 52 without recreating cards
- `Deck_Cleanup()`: Cleans up and frees all 52 cards
- **Usage Pattern**: Create deck → shuffle → draw cards → attach physics → add to DOM
- Cards drawn from deck need `Card_AttachPhysics()` before adding to world

**PokerTable** (`poker_table.h/c`):
- Inherits from Interactable
- Component hierarchy: PokerTable → Interactable → Object
- Manages poker game state and players
- Contains a `Deck` instance for dealing cards
- Stores up to 8 players in `Player* players[MAX_PLAYERS]` array
- Tracks `playerCount` for number of seated players
- Has `size` (Vector3) and `color` for rendering
- `PokerTable_Init()`: Initialize table with position, size, and color
- `PokerTable_AddPlayer()`: Add player to table (returns false if full)
- `PokerTable_RemovePlayer()`: Remove player from table
- `PokerTable_HasPlayer()`: Check if player is seated
- `PokerTable_InteractWithPlayer()`: Manual interaction with player reference
- `PokerTable_Draw()`: Renders brown table with green felt top
- `PokerTable_Cleanup()`: Cleans up deck and clears player references
- `PokerTable_GetType()`: Returns "poker_table"
- Default interaction callback shuffles deck and prints message
- **Rendering**: Brown rectangular slab with darker green felt surface on top
- **Note**: Player references are stored but not owned (not freed on cleanup)

### Physics System

**PhysicsWorld** (`physics.h/c`):
- Wrapper around ODE (Open Dynamics Engine)
- Contains `dWorldID`, `dSpaceID`, and `dJointGroupID`
- `Physics_Init()`: Initializes ODE world with gravity
- `Physics_Step(deltaTime)`: Steps physics simulation
- `Physics_NearCallback()`: Handles collision detection
- `Physics_Cleanup()`: Destroys ODE world and resources
- Gravity set to (0, -9.81, 0)

**RigidBody** (`rigidbody.h/c`):
- Component for physics-enabled objects
- Contains ODE `dBodyID` and `dGeomID`
- Reference to `PhysicsWorld*`
- `RigidBody_InitBox()`: Initialize box-shaped rigid body with mass
- `RigidBody_InitSphere()`: Initialize sphere-shaped rigid body with mass
- `RigidBody_Update()`: Syncs Object position with physics body position
- `RigidBody_GetRotationMatrix()`: Returns raylib Matrix from ODE rotation
- `RigidBody_Cleanup()`: Destroys ODE body and geometry
- Automatically handles position and rotation sync

**Plane** (`plane.h/c`):
- Represents ground plane with physics collision
- Has `Vector2 size` and `Color` for rendering
- Contains ODE `dGeomID` (static geometry, no body)
- `Plane_Init()`: Creates infinite plane geometry for collision
- `Plane_Draw()`: Renders plane as 3D rectangle
- `Plane_Cleanup()`: Destroys ODE geometry
- `Plane_GetType()`: Returns "plane"
- Used for ground and static collision surfaces

### DOM (Document Object Model)

**DOM Structure** (`dom.h`, implemented in `main.c`):
```c
typedef struct {
    Object** objects;
    int count;
    int capacity;
} DOM;
```

**DOM Functions**:
- `DOM_Init(DOM* dom, int initialCapacity)`: Initialize DOM with capacity
- `DOM_AddObject(DOM* dom, Object* obj)`: Add object to DOM
- `DOM_RemoveObject(DOM* dom, Object* obj)`: Remove object from DOM (shifts array)
- `DOM_Cleanup(DOM* dom)`: Free DOM array (not objects themselves)

**DOM Usage**:
- All scene objects (player, cards, chips, planes, etc.) are tracked in the DOM
- Player is added to DOM at initialization
- Ground plane is added to DOM
- Spawners create objects and add them to DOM automatically
- When items are picked up:
  1. Add to player's inventory
  2. Remove from DOM
  3. Deactivate item
- DOM is iterated for rendering, updates, and cleanup

### Spawner System

**Spawner** (`spawner.h/c`):
- Component for spawning objects in random positions
- Has `SpawnType` enum: `SPAWN_TYPE_CARD`, `SPAWN_TYPE_CHIP`
- Has `radius` field for spawn area
- Position determines spawn center
- `Spawner_Init()`: Initialize spawner with position and radius
- `Spawner_SpawnCards(suit, rank, count, physics, dom)`: Spawns cards within radius
- `Spawner_SpawnChips(value, count, physics, dom)`: Spawns chips within radius
- Automatically allocates objects with `malloc()`
- Automatically initializes objects with physics
- Automatically adds objects to DOM
- Spawns objects at random positions within radius from center
- Objects spawn above ground with initial height

### Rendering Utilities

**render_utils** (`render_utils.h/c`):
- `DrawTextBillboard()`: Renders text facing camera (billboarded)
- `DrawText3D()`: Renders text in 3D space without billboarding
- Shared utilities for 3D text rendering

### Important Rendering Notes

**RenderTexture2D Usage**:
- Cards and chips use `BeginTextureMode/EndTextureMode` to render text to textures
- Textures are created once during initialization, not every frame
- When drawing as 2D icons: use negative height in source rectangle to flip upside-down textures
- When drawing in 3D: normal UV coordinates (0,0 → 1,1) work correctly
- Cards and chips must call cleanup functions to unload textures

**3D Text Rendering**:
- DO NOT use `DrawText()` with rlgl matrix transforms - it doesn't work correctly
- Use texture-based approach: render text to RenderTexture2D, then draw as textured quad
- This is more efficient and avoids distortion issues

**2D UI Rendering**:
- Inventory icons use `DrawTexturePro()` with negative source height
- Example: `Rectangle sourceRec = { 0, 0, width, -height }`
- This flips the texture right-side up for UI display

**3D Physics Object Rendering**:
- Cards and chips use rotation matrices from ODE
- `rlPushMatrix()` / `rlPopMatrix()` for local transforms
- `rlMultMatrixf()` applies ODE rotation matrix
- Render geometry between matrix operations
- Automatically synced with physics simulation

### Memory Management

**Dynamic Allocation**:
- Cards and chips are dynamically allocated with `malloc()`
- Inventory uses dynamic array that auto-expands
- DOM uses dynamic array of Object pointers
- Spawners handle allocation automatically
- Must call cleanup functions before `free()`:
  - `Card_Cleanup()` for each card
  - `Chip_Cleanup()` for each chip
  - `RigidBody_Cleanup()` for rigid bodies
  - `Plane_Cleanup()` for planes
  - `Inventory_Cleanup()` for inventory
  - `Physics_Cleanup()` for physics world
  - `DOM_Cleanup()` for DOM
- Cleanup order: Component cleanups → free → CloseWindow()

**Object Lifecycle**:
1. Create object with `malloc()` (or let Spawner do it)
2. Initialize object (e.g., `Card_Init()`, `Chip_Init()`)
3. Add to DOM with `DOM_AddObject()`
4. Object exists in world with physics
5. `Update()` syncs physics body position each frame
6. On pickup: Add to inventory, then `DOM_RemoveObject()`
7. On cleanup: `Inventory_Cleanup()` or manual cleanup handles freeing

**Physics Lifecycle**:
1. Initialize `PhysicsWorld` before creating any physics objects
2. Create RigidBody components during object initialization
3. Call `Physics_Step()` each frame before updating objects
4. Call `RigidBody_Update()` to sync positions after physics step
5. Cleanup rigid bodies before objects
6. Cleanup physics world last

### Controls
- **WASD**: Move around
- **Mouse**: Look around (cursor locked by default)
- **U**: Toggle cursor lock/unlock
- **E**: Interact with closest highlighted object (pickup items)
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

### Deck Usage Example
```c
// Initialize deck
Deck deck;
Deck_Init(&deck);

// Shuffle the deck
srand(time(NULL));  // Seed random number generator
Deck_Shuffle(&deck);

// Draw cards and spawn them in the world
for (int i = 0; i < 5; i++) {
    Card* card = Deck_Draw(&deck);
    if (card) {
        // Attach physics to the card
        Vector3 spawnPos = { i * 1.0f, 2.0f, 0.0f };
        Card_AttachPhysics(card, spawnPos, &physics);
        
        // Add to DOM
        DOM_AddObject(&dom, (Object*)card);
    }
}

// Later, cleanup the entire deck (including any undrawn cards)
Deck_Cleanup(&deck);
```

### Main Game Loop Pattern
```c
// Initialization
PhysicsWorld physics;
Physics_Init(&physics);

DOM dom;
DOM_Init(&dom, 50);

Player player;
Player_Init(&player, startPos);
DOM_AddObject(&dom, (Object*)&player);

// Create ground plane
Plane groundPlane;
Plane_Init(&groundPlane, (Vector3){0, 0, 0}, (Vector2){50, 50}, LIGHTGRAY, &physics);
DOM_AddObject(&dom, (Object*)&groundPlane);

// Create spawner
Spawner spawner;
Spawner_Init(&spawner, (Vector3){0, 2, 0}, 2.0f);

// Spawn objects (spawner handles allocation and DOM addition)
Spawner_SpawnCards(&spawner, SUIT_SPADES, RANK_ACE, 3, &physics, &dom);
Spawner_SpawnChips(&spawner, 5, 10, &physics, &dom);

SetTargetFPS(60);

// Game loop
while (!WindowShouldClose()) {
    float deltaTime = GetFrameTime();
    
    // Handle cursor toggle
    if (IsKeyPressed(KEY_U)) { /* toggle cursor */ }
    
    // Physics step
    Physics_Step(&physics, deltaTime);
    
    // Update all objects in DOM
    for (int i = 0; i < dom.count; i++) {
        Object* obj = dom.objects[i];
        const char* type = obj->GetType(obj);
        
        if (strncmp(type, "card_", 5) == 0) {
            Card_Update((Card*)obj);
        } else if (strncmp(type, "chip_", 5) == 0) {
            Chip_Update((Chip*)obj);
        } else if (strcmp(type, "player") == 0) {
            Player_Update((Player*)obj, deltaTime);
        }
    }
    
    // Find closest interactable from DOM
    // Handle E key interaction
    // On pickup: Inventory_AddItem + DOM_RemoveObject
    
    // Draw
    BeginDrawing();
    ClearBackground(RAYWHITE);
    
    BeginMode3D(*Player_GetCamera(&player));
        // Draw objects from DOM
        for (int i = 0; i < dom.count; i++) {
            // Draw based on type
        }
    EndMode3D();
    
    // Draw UI
    Player_DrawInventoryUI(&player);
    DrawFPS(10, screenHeight - 30);
    
    EndDrawing();
}

// Cleanup
// Free all allocated objects with their cleanup functions
for (int i = 0; i < dom.count; i++) {
    // Call appropriate cleanup based on type
}
DOM_Cleanup(&dom);
Physics_Cleanup(&physics);
CloseWindow();
```

### Type System
Objects use a virtual function pattern for runtime type identification:
- Each component implements `GetType()` function
- Returns string identifier: "player", "plane", "card_ace_spades", "chip_5"
- Used for type-specific behavior in DOM iteration
- Use `strncmp(type, "card_", 5)` for prefix matching
- Use `strcmp(type, "player")` for exact matching

### Common Pitfalls

1. **Text Rendering**: Never try to render 2D `DrawText()` in 3D with matrix transforms - use RenderTexture2D instead

2. **Memory**: Always cleanup RenderTextures before freeing cards/chips, cleanup rigid bodies, and use DOM_Cleanup or Inventory_Cleanup to properly free objects

3. **Physics Order**: Always call `Physics_Step()` before updating objects, then call `RigidBody_Update()` to sync positions

4. **Player Camera**: Player contains its own camera - use `Player_GetCamera()` to get it for rendering

5. **Component Access**: Access nested members like `card.base.base.base.position` (Card → Item → Interactable → Object)

6. **Upside-Down Icons**: Use negative height in source rectangle when drawing RenderTexture2D as 2D UI icons

7. **DOM Removal**: Always remove objects from DOM when picking them up, otherwise they'll be rendered and updated despite being in inventory

8. **Item Inheritance**: All inventory-compatible objects must inherit from Item, not directly from Interactable

9. **Forward Declarations**: Use `struct Item;` forward declarations to avoid typedef redefinition warnings between inventory.h and item.h

10. **Physics World Reference**: RigidBody components need a reference to the PhysicsWorld - pass it during initialization

11. **Spawner Allocation**: Spawners automatically allocate objects with malloc() - don't allocate before calling spawn functions

12. **Type Checking**: Use the `GetType()` virtual function and string comparison for runtime type identification in DOM iterations

13. **Rotation Matrices**: When rendering physics objects, use `RigidBody_GetRotationMatrix()` and apply with `rlMultMatrixf()` for proper 3D orientation
