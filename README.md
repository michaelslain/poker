# Poker

A first-person poker game built with C++ and raylib featuring an inventory system and object-oriented architecture with physics simulation.

## Stack

- C++ (C++17)
- raylib 5.5
- ODE (Open Dynamics Engine) - Physics simulation

## Features

### Core Gameplay
- First-person movement and camera controls
- Interactive card and chip pickup system
- Dynamic inventory with visual UI and selection
- Physics-based object interactions using ODE

### Poker System
- Fully functional Texas Hold'em poker table with up to 8 seats
- AI opponents (Enemy) with thinking delays and random betting logic
- Dealer button rotation and blind management
- Proper betting rounds: Preflop, Flop, Turn, River
- Value-based chip system with automatic denomination optimization
- Community cards rendered on table, hole cards in player inventories
- UI-based betting interface for human player

### Architecture
- Object-oriented class hierarchy with virtual functions and inheritance
- Person base class with Player, Enemy, and Dealer implementations
- Scene DOM (Document Object Model) for object management
- Spawner system for dynamic object creation
- RAII pattern for automatic resource cleanup

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
make clean  # Clean build artifacts
```

### Controls

- **WASD** - Move around
- **Mouse** - Look around (reduced sensitivity)
- **U** - Toggle cursor lock/unlock
- **E** - Interact with objects / Pick up items
- **X** - Toggle item selection in inventory
- **Left/Right Arrow** - Navigate inventory selection
- **[ ]** - Adjust FOV


