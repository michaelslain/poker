## Project Overview
A first-person poker game built with C and raylib using a component-based architecture with inventory system and DOM-based object management.

## Build System
The project uses a Makefile with macOS-specific raylib paths:
- Raylib include path: `/opt/homebrew/opt/raylib/include`
- Raylib library path: `/opt/homebrew/opt/raylib/lib`
- Compiler flags: `-Wall -Wextra -std=c99 -Iinclude`
- Target executable: `game`

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
Raylib must be installed via Homebrew:
```bash
brew install raylib
```

## Code Architecture

### Component Hierarchy
The game uses an inheritance-style component system through struct composition:

```
Object (base)
├── Interactable
│   └── Item
│       └── Card
├── Player (contains GameCamera and Inventory)
└── GameCamera
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
│   ├── inventory.h
│   ├── inventory_ui.h
│   └── render_utils.h
├── src/              # Implementation files
│   ├── object.c
│   ├── camera.c
│   ├── player.c
│   ├── interactable.c
│   ├── item.c
│   ├── card.c
│   ├── inventory.c
│   ├── inventory_ui.c
│   └── render_utils.c
└── main.c            # Game entry point with DOM
```

### Core Components

**Object** (`object.h/c`):
- Base component with position, rotation, scale
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
- Has `Suit` (HEARTS, DIAMONDS, CLUBS, SPADES) and `Rank` (ACE-KING)
- **Texture-based rendering**: Each card generates a RenderTexture2D once during init
- Displays rank (large) and suit name (smaller) on white background
- Colors: Red for hearts/diamonds, black for clubs/spades
- `Card_DrawIcon()`: Renders texture as 2D icon with negative height to flip correctly

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

### DOM (Document Object Model)

**DOM Structure** (`main.c`):
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
- `DOM_RemoveObject(DOM* dom, Object* obj)`: Remove object from DOM
- `DOM_Cleanup(DOM* dom)`: Free all objects and DOM memory

**DOM Usage**:
- All scene objects (player, cards, etc.) are tracked in the DOM
- Player is added to DOM at initialization
- Cards are added to DOM when created
- When items are picked up:
  1. Add to player's inventory
  2. Remove from DOM
  3. Deactivate item
- DOM is iterated for rendering and cleanup

### Rendering Utilities

**render_utils** (`render_utils.h/c`):
- `DrawTextBillboard()`: Renders text facing camera (billboarded)
- `DrawText3D()`: Renders text in 3D space without billboarding
- Shared utilities for 3D text rendering

### Important Rendering Notes

**RenderTexture2D Usage**:
- Cards use `BeginTextureMode/EndTextureMode` to render text to textures
- Textures are created once during initialization, not every frame
- When drawing as 2D icons: use negative height in source rectangle to flip upside-down textures
- When drawing in 3D: normal UV coordinates (0,0 → 1,1) work correctly
- Cards must call `Card_Cleanup()` to unload textures

**3D Text Rendering**:
- DO NOT use `DrawText()` with rlgl matrix transforms - it doesn't work correctly
- Use texture-based approach: render text to RenderTexture2D, then draw as textured quad
- This is more efficient and avoids distortion issues

**2D UI Rendering**:
- Inventory icons use `DrawTexturePro()` with negative source height
- Example: `Rectangle sourceRec = { 0, 0, width, -height }`
- This flips the texture right-side up for UI display

### Memory Management

**Dynamic Allocation**:
- Cards are dynamically allocated with `malloc()`
- Inventory uses dynamic array that auto-expands
- DOM uses dynamic array of Object pointers
- Must call cleanup functions before `free()`:
  - `Card_Cleanup()` for each card
  - `Inventory_Cleanup()` for inventory
  - `DOM_Cleanup()` for DOM
- Cleanup order: Component cleanups → free → CloseWindow()

**Object Lifecycle**:
1. Create object with `malloc()`
2. Initialize object (e.g., `Card_Init()`)
3. Add to DOM with `DOM_AddObject()`
4. Object exists in world
5. On pickup: Add to inventory, then `DOM_RemoveObject()`
6. On cleanup: `Inventory_Cleanup()` or `DOM_Cleanup()` handles freeing

### Controls
- **WASD**: Move around
- **Mouse**: Look around (cursor locked by default)
- **U**: Toggle cursor lock/unlock
- **E**: Interact with closest highlighted object (pickup items)
- **[ ]**: Increase/decrease FOV
- **ESC**: Close window

### Main Game Loop Pattern
```c
// Initialization
DOM dom;
DOM_Init(&dom, 100);

Player player;
Player_Init(&player, startPos);
DOM_AddObject(&dom, (Object*)&player);

// Create cards
for (int i = 0; i < numCards; i++) {
    Card* card = malloc(sizeof(Card));
    Card_Init(card, position, suit, rank);
    DOM_AddObject(&dom, (Object*)card);
}

// Game loop
while (!WindowShouldClose()) {
    // Handle cursor toggle
    if (IsKeyPressed(KEY_U)) { /* toggle cursor */ }
    
    // Update
    Player_Update(&player, deltaTime);
    
    // Find closest interactable from DOM
    // Handle E key interaction
    // On pickup: Inventory_AddItem + DOM_RemoveObject
    
    // Draw
    BeginMode3D(*Player_GetCamera(&player));
        // Draw world geometry
        // Draw cards/interactables from DOM
    EndMode3D();
    
    // Draw UI
    Player_DrawInventoryUI(&player);
}

// Cleanup
DOM_Cleanup(&dom);  // Frees all objects including cards
CloseWindow();
```

### Common Pitfalls

1. **Text Rendering**: Never try to render 2D `DrawText()` in 3D with matrix transforms - use RenderTexture2D instead

2. **Memory**: Always cleanup RenderTextures before freeing cards, and use DOM_Cleanup or Inventory_Cleanup to properly free objects

3. **Player Camera**: Player contains its own camera - use `Player_GetCamera()` to get it for rendering

4. **Component Access**: Access nested members like `card.base.base.base.position` (Card → Item → Interactable → Object)

5. **Upside-Down Icons**: Use negative height in source rectangle when drawing RenderTexture2D as 2D UI icons

6. **DOM Removal**: Always remove objects from DOM when picking them up, otherwise they'll be rendered and updated despite being in inventory

7. **Item Inheritance**: All inventory-compatible objects must inherit from Item, not directly from Interactable

8. **Forward Declarations**: Use `struct Item;` forward declarations to avoid typedef redefinition warnings between inventory.h and item.h
