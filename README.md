# Poker

3D rouge-like puzzle? procedurally generated story game

## Features

- **Procedural Level Generation** - Infinite non-linear casino levels with organic room layouts and increasing difficulty
- **Roguelike Progression** - Start in a hospital (level 0), progress through procedurally generated casino levels
- **Difficulty Scaling** - Exponential difficulty curve affecting enemy count, AI quality, resources, and insanity
- **Alternate Dimensions** - Consume Salvia to enter parallel dimensions with different layouts; exit via stairs to jump forward 1-10 levels
- **Texas Hold'em Poker** - Complete poker implementation with AI opponents, betting, and hand evaluation
- **Insanity System** - Mental state affected by movement, seating, kills, and psychedelic trips; warps FOV from 60° to 150°
- **Psychedelic Trips** - Shrooms: 5-minute trips with visual shader effects (breathing, warping, color shifting); Salvia: Level-long alternate dimension trips with fast come-up/down, inverted FOV, and 100% insanity immunity
- **Physics-Based Gameplay** - ODE physics engine for realistic item interactions and collisions
- **Inventory System** - Dynamic item stacking with automatic sorting by category
- **Weapon Combat** - 6-round revolver with raycast hit detection
- **Consumable Substances** - Weed, Cocaine, Molly, Adrenaline, Salvia, Shrooms, Vodka, Fent (instant death) with unique effects
- **Dynamic Lighting** - Shader-based lighting system supporting up to 32 lights with decorative light bulbs emitting atmospheric blue light
- **Death System** - Die from insanity reaching 100% or fentanyl overdose with 3-second death vignette effect

## Stack

- C++ (C++17)
- raylib 5.5
- ODE (Open Dynamics Engine) - Physics simulation

## Dev

### Prerequisites

Install raylib, ODE, and ccache:
```bash
brew install raylib
brew install ode
brew install ccache  # For faster compilation
```

### Build and Run

```bash
make run-debug    # Build in debug mode and run
make debug        # Just build debug mode
make run          # Build in release mode and run
make release      # Just build release mode
make test         # Run all unit tests
make clean        # Clean build artifacts
```

### Controls

- **WASD** - Move around
- **Mouse** - Look around (0.001 sensitivity)
- **U** - Toggle cursor lock/unlock
- **E** - Interact with objects / Pick up items
- **X** - Toggle item selection in inventory
- **Left/Right Arrow** - Navigate inventory selection
- **Left Mouse** - Use held item (shoot pistol, consume substance)
- **C** - Toggle collision debug visualization
- **[ ]** - Adjust FOV manually

## Architecture

### Core Patterns
- **Object-oriented hierarchy** - Virtual functions and inheritance for polymorphic behavior
- **DOM (Document Object Model)** - Centralized scene graph for all game objects
- **RAII (Resource Acquisition Is Initialization)** - Automatic resource cleanup via constructors/destructors
- **Polymorphic cloning** - Virtual `Clone()` method for object spawning without type checking
- **Hierarchical type system** - Type strings include full inheritance chain (e.g., `"object_interactable_item_chip_50"`)

### Class Hierarchy
```
Object (base class)
├── Interactable
│   ├── Item (pickupable objects)
│   │   ├── Card
│   │   ├── Chip
│   │   ├── Weapon (abstract base)
│   │   │   └── Pistol (6-round revolver)
│   │   └── Substance (abstract base)
│   │       ├── Weed
│   │       ├── Cocaine
│   │       ├── Molly
│   │       ├── Adrenaline
│   │       ├── Salvia
│   │       ├── Shrooms
│   │       ├── Vodka
│   │       └── Fent (instant death)
│   └── PokerTable
├── Person (abstract base with inventory)
│   ├── Player (human-controlled with insanity system)
│   ├── Enemy (AI)
│   └── Dealer (NPC)
├── Light (base for lighting)
│   └── LightBulb (point light with decorative geometry)
├── Floor / Ceiling / Wall (geometry)
├── RigidBody (physics-enabled objects)
├── Spawner (object spawning)
└── ChipStack (chip management)

Standalone Classes:
├── GameCamera (first-person camera)
├── PhysicsWorld (ODE wrapper)
├── DOM (scene graph manager)
├── Inventory (item storage)
├── Deck (card deck)
├── Collider (physics collision component)
├── Scene (scene data)
├── SceneManager (singleton scene switching)
├── LightingManager (static lighting shader manager)
├── PsychedelicManager (static psychedelic shader manager)
└── InsanityManager (player mental state system)
```

### Key Systems
- **Physics** - ODE integration for rigid body dynamics and collision detection
- **Inventory** - Dynamic item stacking with automatic sorting
- **Poker game logic** - Complete Texas Hold'em implementation with betting, hand evaluation, and showdown
- **Lighting** - `LightingManager` static class managing shader-based lighting with up to 4 dynamic lights
- **Psychedelic system** - `PsychedelicManager` with post-processing shaders supporting multiple trip types: Shrooms (5-minute duration with 60s come-up, 120s peak, 120s come-down) and Salvia (5s come-up, level-long peak, 5s come-down)
- **Insanity system** - `InsanityManager` tracking player mental state based on movement, seating, kills, and psychedelic trips; affects FOV (60°-150°) and compounds with trip intensity
- **Death system** - Player-managed death state triggered by 100% insanity or fentanyl overdose; 3-second vignette animation with shader effects
- **Scene management** - Scene system for different game states
- **Level system** - Procedural level generation with difficulty scaling and alternate dimensions (Salvia mechanic)
- **Testing** - Catch2 v3.5.0 framework with 224 test cases (1449 assertions) covering all classes including PsychedelicManager and Salvia
