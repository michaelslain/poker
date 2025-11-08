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
- **[ ]** - Adjust FOV

## Testing

The project includes comprehensive unit tests for all classes using Catch2 v3.5.0.

### Running Tests

```bash
make test   # Build and run all tests
```

### Test Coverage

- 28 test files covering all game classes
- Tests for base classes (Object, Person, Interactable, Item)
- Tests for game objects (Card, Chip, Deck, PokerTable)
- Tests for systems (DOM, Inventory, Physics, Spawner)
- Tests for rendering (Camera, Light, RenderUtils)
- Regression tests for previously fixed bugs

### Benefits

- Catch bugs before gameplay testing
- Ensure fixed bugs stay fixed
- Document expected class behavior
- Enable safe refactoring
- Validate edge cases and boundary conditions

See `CLAUDE.md` for detailed testing documentation.


