# Poker

3D rouge-like puzzle? procedurally generated story game

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
- **Mouse** - Look around (reduced sensitivity)
- **U** - Toggle cursor lock/unlock
- **E** - Interact with objects / Pick up items
- **X** - Toggle item selection in inventory
- **Left/Right Arrow** - Navigate inventory selection
- **Left Mouse** - Shoot (when holding pistol)

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
│   │       └── Vodka
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
└── SceneManager (singleton scene switching)
```

### Key Systems
- **Physics** - ODE integration for rigid body dynamics and collision detection
- **Inventory** - Dynamic item stacking with automatic sorting
- **Poker game logic** - Complete Texas Hold'em implementation with betting, hand evaluation, and showdown
- **Lighting** - `LightingManager` static class managing shader-based lighting with up to 4 dynamic lights
- **Psychedelic system** - `PsychedelicManager` with post-processing shaders for shrooms trips (5-minute duration with come-up, peak, and come-down stages)
- **Insanity** - Player mental state system affecting FOV based on movement
- **Scene management** - Scene system for different game states
- **Testing** - Catch2 v3.5.0 framework with 114 test cases (725 assertions) covering all classes
