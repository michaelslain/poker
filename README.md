# Poker

3D rouge-like puzzle? procedurally generated story game

## Stack

- C++ (C++17)
- raylib 5.5
- ODE (Open Dynamics Engine) - Physics simulation

## Dev

### Prerequisites

Install raylib and ODE:
```bash
brew install raylib
brew install ode
```

### Build and Run

```bash
make        # Compile the project
make run    # Build and run
make test   # Run all unit tests
make clean  # Clean build artifacts
```

### Controls

- **WASD** - Move around
- **Mouse** - Look around (reduced sensitivity)
- **U** - Toggle cursor lock/unlock
- **E** - Interact with objects / Pick up items
- **X** - Toggle item selection in inventory
- **Left/Right Arrow** - Navigate inventory selection
- **[** - Decrease FOV
- **]** - Inrease FOV


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
│   │   └── Pistol
│   └── PokerTable
├── Person (abstract base with inventory)
│   ├── Player (human-controlled)
│   ├── Enemy (AI)
│   └── Dealer (NPC)
├── Floor / Ceiling / Wall (geometry)
├── LightBulb (lighting)
├── Spawner (object spawning)
└── ChipStack (chip management)
```

### Key Systems
- **Physics** - ODE integration for rigid body dynamics and collision detection
- **Inventory** - Dynamic item stacking with automatic sorting
- **Poker game logic** - Complete Texas Hold'em implementation with betting, hand evaluation, and showdown
- **Scene management** - Scene system for different game states
- **Testing** - Catch2 v3.5.0 framework with 100+ unit tests covering all classes
