# Poker

A first-person poker game built with C++ and raylib featuring an inventory system and object-oriented architecture with physics simulation.

## Stack

- C++ (C++17)
- raylib 5.5
- ODE (Open Dynamics Engine) - Physics simulation

## Features

- First-person movement and camera controls
- Interactive card and chip pickup system
- Dynamic inventory with visual UI and selection
- Object-oriented class hierarchy with virtual functions
- Physics-based object interactions
- Poker table with deck management
- Scene DOM (Document Object Model)
- Spawner system for dynamic object creation

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

## Architecture

Built with OOP principles:
- **Virtual functions** for polymorphic behavior
- **Inheritance hierarchy**: Object → Interactable → Item → Card/Chip
- **RAII** pattern with constructors/destructors
- **std::vector** and **std::array** for dynamic collections
- **Physics integration** with ODE for realistic interactions
