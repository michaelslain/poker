## Project Overview
A first-person poker game built with C++ and raylib using object-oriented architecture with inheritance, inventory system, DOM-based object management, and ODE physics engine integration.

## Build System
The project uses a Makefile with macOS-specific paths:
- Raylib include path: `/opt/homebrew/opt/raylib/include`
- Raylib library path: `/opt/homebrew/opt/raylib/lib`
- ODE include path: `/opt/homebrew/opt/ode/include`
- ODE library path: `/opt/homebrew/opt/ode/lib`
- Compiler: `g++` with `-std=c++17`
- Compiler flags: `-Wall -Wextra -std=c++17 -Iinclude`
- Target executable: `game`
- Links against: `-lraylib -lode -lm -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo`

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
- ODE include path
- Project include path (`-I/Users/michaelslain/Documents/dev/poker/include`)
- C++ language mode (`-x c++`)

### Prerequisites
Raylib and ODE must be installed via Homebrew:
```bash
brew install raylib
brew install ode
```

## Code Architecture

### Object-Oriented Class Hierarchy
The game uses C++ inheritance with virtual functions for polymorphism:

```
Object (base class with virtual functions)
├── Interactable
│   ├── Item
│   │   ├── Card (with RigidBody*)
│   │   └── Chip (with RigidBody*)
│   └── PokerTable (poker game manager)
├── Person (abstract base with Inventory)
│   ├── Player (human player with GameCamera)
│   ├── Enemy (AI player)
│   └── Dealer (NPC)
├── Plane (ground plane with physics collision)
├── RigidBody (physics-enabled objects)
└── Spawner (creates and manages spawned objects)

GameCamera (standalone class)
PhysicsWorld (standalone class)
DOM (Document Object Model - manages all objects)
Inventory (dynamic item collection)
Deck (card deck management)
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
```
poker/
├── include/          # Header files (.hpp)
│   ├── object.hpp
│   ├── camera.hpp
│   ├── player.hpp
│   ├── interactable.hpp
│   ├── item.hpp
│   ├── card.hpp
│   ├── chip.hpp
│   ├── deck.hpp
│   ├── poker_table.hpp
│   ├── inventory.hpp
│   ├── inventory_ui.hpp
│   ├── render_utils.hpp
│   ├── dom.hpp
│   ├── spawner.hpp
│   ├── physics.hpp
│   ├── rigidbody.hpp
│   └── plane.hpp
├── src/              # Implementation files (.cpp)
│   ├── object.cpp
│   ├── camera.cpp
│   ├── player.cpp
│   ├── interactable.cpp
│   ├── item.cpp
│   ├── card.cpp
│   ├── chip.cpp
│   ├── deck.cpp
│   ├── poker_table.cpp
│   ├── inventory.cpp
│   ├── inventory_ui.cpp
│   ├── render_utils.cpp
│   ├── spawner.cpp
│   ├── physics.cpp
│   ├── rigidbody.cpp
│   └── plane.cpp
└── main.cpp          # Game entry point with DOM
```

### Core Components

**Object** (`object.hpp/cpp`):
- Base class with position, rotation, scale
- `isActive` flag for lifecycle management
- Virtual functions: `Update(float deltaTime)`, `Draw(Camera3D camera)`, `GetType() const`
- All other components inherit from this
- Uses RAII pattern

**Person** (`person.hpp/cpp`):
- Inherits from Object
- Abstract base class for Player, Enemy, and Dealer
- Contains `Inventory` instance for chip and card management
- `name` field for identification
- `isSitting` flag for seated state
- `height` field (customizable, e.g., enemies are 1.5x taller)
- Virtual `PromptBet()` for betting decisions (returns -1 for thinking, 0=fold, 1=call, 2=raise)
- `SitDown()` and `StandUp()` for seating management
- Provides common interface for poker table interaction
- Returns type string in format: "player", "enemy", "dealer"

**Player** (`player.hpp/cpp`):
- Inherits from Person
- First-person controller with WASD movement
- Mouse look with reduced sensitivity (0.001f)
- Contains `GameCamera` instance
- FOV control with `[` and `]` keys
- Crosshair-based interaction system (raycasting)
- Pickup behavior: adds items to inventory, removes from DOM
- Inventory selection: X key to select/deselect, arrow keys to navigate
- ODE physics capsule geometry for collision
- Proper collision detection with poker table using `dCollide()`
- Betting UI: Shows fold/call/raise options with slider for raise amount
- Overrides `PromptBet()` to use UI-based betting decisions
- Returns type: "player"

**Enemy** (`enemy.hpp/cpp`):
- Inherits from Person
- AI-controlled poker player
- Thinking timer system: 2-4 second random delay before betting decisions
- Simple AI logic: Random choice between fold/call/raise
- Checks total chip value (not stack count) for affordability
- Taller than player (1.5x normal height) for visual distinction
- Overrides `PromptBet()` with state machine (thinking → decision)
- Returns type: "enemy"

**Dealer** (`dealer.hpp/cpp`):
- Inherits from Person
- Non-player character positioned at poker table
- Does not participate in poker hands
- Visual presence only (no AI or betting logic)
- Owned as child object by PokerTable
- Returns type: "dealer"

**GameCamera** (`camera.hpp/cpp`):
- Wraps raylib's Camera3D
- Tracks pitch/yaw angles (Vector2)
- `AdjustFOV()` handles FOV controls
- `SetTarget()` positions camera
- `GetCamera()` returns pointer to internal Camera3D
- Eye-level offset: 1.7 units above player position

**Interactable** (`interactable.hpp/cpp`):
- Inherits from Object
- Objects player can interact with (press E)
- `interactRange` field (default 3.0 units)
- `std::function<void(Interactable*)> onInteract` callback
- `Interact()` virtual function
- `DrawPrompt(Camera3D)` renders "E" prompt billboard
- Default behavior defined by callback

**Item** (`item.hpp/cpp`):
- Inherits from Interactable
- Bridge between Interactable and inventory-compatible objects
- `DrawIcon(Rectangle destRect)` virtual function for UI rendering
- All inventory-compatible objects must inherit from Item

**Card** (`card.hpp/cpp`):
- Inherits from Item
- Component hierarchy: Card → Item → Interactable → Object
- Uses C-style enums: `Suit` (SUIT_HEARTS, SUIT_DIAMONDS, SUIT_CLUBS, SUIT_SPADES)
- Uses C-style enums: `Rank` (RANK_ACE - RANK_KING)
- Contains `RigidBody*` for physics (optional, can be nullptr)
- `RenderTexture2D texture` for card face rendering
- Constructor: `Card(Suit, Rank, Vector3 pos, PhysicsWorld* physics)`
- Destructor: Automatically cleans up texture and rigid body
- `AttachPhysics()` for adding physics after creation
- `Update()` syncs with physics body
- `Draw()` renders as 3D quad with texture
- `DrawIcon()` renders as 2D icon (flipped)
- Static utility functions: `GetSuitSymbol()`, `GetRankString()`, `GetSuitColor()`

**Chip** (`chip.hpp/cpp`):
- Inherits from Item
- Integer `value` field (1, 5, 10, 25, 100)
- Contains `RigidBody*` for physics
- Color-coded: WHITE(1), RED(5), BLUE(10), GREEN(25), BLACK(100)
- `RenderTexture2D iconTexture` for chip rendering
- Constructor: `Chip(int value, Vector3 pos, PhysicsWorld* physics)`
- Destructor: Automatically cleans up texture and rigid body
- `Update()` syncs with physics body
- `Draw()` renders as 3D cylinder
- `DrawIcon()` renders as 2D icon
- Static function: `GetColorFromValue()`

**Inventory** (`inventory.hpp/cpp`):
- Uses `std::vector<ItemStack>` for dynamic storage
- `ItemStack` contains: `Item* item`, `int count`, `std::string typeString`
- `AddItem(Item*)` adds or increments stack
- `RemoveItem(int stackIndex)` decrements or removes stack
- Accessors: `GetStackCount()`, `GetStack(int index)`, `GetStacks()`
- Automatic cleanup in destructor

**InventoryUI** (`inventory_ui.hpp/cpp`):
- `InventoryUI_Draw(Inventory*, int selectedIndex)` function
- Renders items as 60x60 icons at top-left
- White outline for selected item
- Displays stack count if > 1
- Delegates to `Item::DrawIcon()` polymorphically

**Deck** (`deck.hpp/cpp`):
- NOT an Object - standalone class
- Uses `std::array<Card*, DECK_SIZE>` (52 cards)
- Constructor automatically creates all 52 cards (no physics)
- `Shuffle()` uses Fisher-Yates algorithm
- `Draw()` returns top card, decrements count
- `Peek()` returns top card without removing
- `Reset()` resets count to 52
- Destructor cleans up all cards

**PokerTable** (`poker_table.hpp/cpp`) - **SIMPLIFIED DESIGN (398 lines)**:
- Inherits from Interactable
- Manages 8 seats with Person* occupants (Player or Enemy)
- **Dual-reference pattern**: Deck and community cards are both attributes (for game logic) AND children (for DOM rendering)
- Deck* and Dealer* owned as child objects
- ODE box geometry (`dGeomID`) for collision
- Simple state: `handActive`, `pot`, `currentBet`, `currentSeat`, `bettingActive`
- Constructor: `PokerTable(Vector3 pos, Vector3 size, Color color, PhysicsWorld*)`
- Virtual functions: `Update()`, `Draw()`, `Interact()`, `GetType()`
- Seating: `SeatPerson()`, `UnseatPerson()`, `FindSeatIndex()`, `FindClosestOpenSeat()`
- Renders as brown box with green felt top

**Simplified Poker Game Logic**:
- **Seat management**: Simple `Seat` struct with `Person* occupant`, `currentBet`, `hasFolded`
- **Dealer button**: Rotates with `NextOccupiedSeat()` helper
- **Blinds**: Posted in `StartHand()` via `TakeChips()` helper (SB=5, BB=10)
- **Hole cards**: Deal 2 cards per player directly into inventories (NOT stored in separate array)
- **Community cards**: Stored in `std::vector<Card*>` AND added as children via `AddChild()` for rendering. Positioned in a line (0.6 spacing) offset to the side of the table (left side, -0.3z offset) and raised 0.15 above table surface for visibility
- **Betting**: Single `ProcessBetting(dt)` called each frame handles all betting logic
- **Value-based chips**: `CountChips()`, `TakeChips()`, `GiveChips()` helpers use inventory system
- **Hand flow**: `StartHand()` → betting → `DealFlop()` → betting → `DealTurn()` → betting → `DealRiver()` → betting → `EndHand()`
- **Winner**: First non-folded player found (simplified, no hand evaluation yet)
- **Cleanup**: `EndHand()` removes hole cards from inventories, deletes them, starts next hand

**Key Implementation Details**:
- **No separate hole cards tracking**: Cards only exist in player inventories
- **Dual-reference for rendering**: `deck` pointer + `AddChild(deck)` so DOM renders it
- **Community cards**: `communityCards.push_back(card)` + `AddChild(card)` for dual-reference. Cards positioned at `{startX + (i * 0.6f), tableY + 0.15f, tableZ - 0.3f}` for flop (i=0-2), turn (i=3), river (i=4), starting from `position.x - 1.0f`
- **Mid-hand joining**: Players marked as `hasFolded = true` if joining during active hand
- **Person::PromptBet()**: Returns -1 (thinking), 0 (fold), 1 (call), or 2 (raise)
- **Enemy thinking**: 2-4 second timer before returning action
- **Player betting UI**: Displays fold/call/raise buttons with values
- **Chip affordability**: Checks `CountChips(person)` total value, not stack count
- **Automatic progression**: Hands start automatically when 2+ players seated

### Physics System

**PhysicsWorld** (`physics.hpp/cpp`):
- Wrapper around ODE (Open Dynamics Engine)
- Contains `dWorldID`, `dSpaceID`, `dJointGroupID`
- Constructor initializes ODE with gravity (0, -9.81, 0)
- Destructor cleans up ODE resources
- `Step(float deltaTime)` advances physics simulation
- Static `NearCallback()` for collision handling
- Handles contact joints and surface properties

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
- `float radius` for spawn area
- Constructor: `Spawner(Vector3 pos, float radius)`
- `SpawnCards(Suit, Rank, count, PhysicsWorld*, DOM*)` spawns cards randomly
- `SpawnChips(int value, count, PhysicsWorld*, DOM*)` spawns chips randomly
- Automatically allocates objects with `new`
- Automatically initializes with physics
- Automatically adds to DOM
- Sets `isDynamicallyAllocated = true` flag

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
    if (obj->isDynamicallyAllocated) {
        delete obj;  // Destructor handles cleanup
    }
}
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
table->isDynamicallyAllocated = true;
dom.AddObject(table);

// Spawn objects
Spawner spawner({0, 2, 0}, 2.0f);
spawner.SpawnCards(SUIT_SPADES, RANK_ACE, 3, &physics, &dom);
spawner.SpawnChips(5, 10, &physics, &dom);

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
    if (obj->isDynamicallyAllocated) {
        delete obj;  // Calls destructor
    }
}
delete player;
delete ground;

// PhysicsWorld destructor called automatically
```

### Type System
Objects use virtual `GetType() const` for runtime type identification:
- Returns string: "player", "plane", "card_hearts_ace", "chip_5", "poker_table"
- Use `strncmp(type, "card_", 5)` for prefix matching
- Use `strcmp(type, "player")` for exact matching
- Polymorphic - overridden in each derived class

### Common Patterns

**Creating Objects**:
```cpp
// Direct allocation
Card* card = new Card(SUIT_SPADES, RANK_ACE, {0, 2, 0}, &physics);
card->isDynamicallyAllocated = true;
dom.AddObject(card);

// Or use Spawner (recommended)
spawner.SpawnCards(SUIT_HEARTS, RANK_KING, 5, &physics, &dom);
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

9. **Spawner Flag**: Spawned objects have `isDynamicallyAllocated = true`

10. **Constructor Initialization**: Use member initializer lists for efficiency
