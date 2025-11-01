# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A first-person poker game built with C and raylib using a component-based architecture.

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
│   └── Card
├── Player (contains GameCamera)
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
│   ├── card.h
│   └── render_utils.h
├── src/              # Implementation files
│   ├── object.c
│   ├── camera.c
│   ├── player.c
│   ├── interactable.c
│   ├── card.c
│   └── render_utils.c
└── main.c            # Game entry point
```

### Core Components

**Object** (`object.h/c`):
- Base component with position, rotation, scale
- All other components inherit from this

**Player** (`player.h/c`):
- First-person controller with WASD movement
- Mouse look (inverted: moving mouse left pans left, moving mouse up pans up)
- Contains its own `GameCamera` instance
- FOV control with `[` and `]` keys
- No jumping implemented yet

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

**Card** (`card.h/c`):
- Inherits from Interactable
- Has `Suit` (HEARTS, DIAMONDS, CLUBS, SPADES) and `Rank` (ACE-KING)
- **Texture-based rendering**: Each card generates a RenderTexture2D once during init
- Displays rank (large) and suit name (smaller) on white background
- Colors: Red for hearts/diamonds, black for clubs/spades

### Rendering Utilities

**render_utils** (`render_utils.h/c`):
- `DrawTextBillboard()`: Renders text facing camera (billboarded)
- `DrawText3D()`: Renders text in 3D space without billboarding
- Shared utilities for 3D text rendering

### Important Rendering Notes

**RenderTexture2D Usage**:
- Cards use `BeginTextureMode/EndTextureMode` to render text to textures
- Textures are created once during initialization, not every frame
- Normal UV coordinates (0,0 → 1,1) work correctly when rendering the texture to 3D quads
- Cards must call `Card_Cleanup()` to unload textures

**3D Text Rendering**:
- DO NOT use `DrawText()` with rlgl matrix transforms - it doesn't work correctly
- Use texture-based approach: render text to RenderTexture2D, then draw as textured quad
- This is more efficient and avoids distortion issues

### Memory Management

**Dynamic Allocation**:
- Cards are dynamically allocated with `malloc()`
- No fixed MAX_CARDS limit
- Must call `Card_Cleanup()` for each card before `free()`
- Cleanup order: Card_Cleanup → free(cards) → CloseWindow()

### Controls

- **WASD**: Move around
- **Mouse**: Look around (cursor locked by default)
- **U**: Toggle cursor lock/unlock
- **E**: Interact with closest highlighted object
- **[ ]**: Increase/decrease FOV
- **ESC**: Close window

### Main Game Loop Pattern

```c
// Initialization
Player player;
Player_Init(&player, startPos);

Card* cards = malloc(sizeof(Card) * capacity);
// Initialize cards...

// Game loop
while (!WindowShouldClose()) {
    // Handle cursor toggle
    if (IsKeyPressed(KEY_U)) { /* toggle cursor */ }
    
    // Update
    Player_Update(&player, deltaTime);
    
    // Find closest interactable
    // Handle E key interaction
    
    // Draw
    BeginMode3D(*Player_GetCamera(&player));
        // Draw world geometry
        // Draw cards/interactables
    EndMode3D();
}

// Cleanup
for (cards) Card_Cleanup(&cards[i]);
free(cards);
CloseWindow();
```

### Common Pitfalls

1. **Text Rendering**: Never try to render 2D `DrawText()` in 3D with matrix transforms - use RenderTexture2D instead
2. **Memory**: Always cleanup RenderTextures before freeing cards
3. **Player Camera**: Player contains its own camera - use `Player_GetCamera()` to get it for rendering
4. **Component Access**: Access nested members like `card.base.base.position` (Card → Interactable → Object)
