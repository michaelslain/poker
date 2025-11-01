# Poker

A first-person poker game built with C and raylib featuring an inventory system and component-based architecture.

## Stack

- C (C99)
- raylib 5.5
- ODE (Open Dynamics Engine) - Physics simulation

## Features

- First-person movement and camera controls
- Interactive card pickup system
- Dynamic inventory with visual UI
- Component inheritance system
- Scene DOM (Document Object Model)

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
- **Mouse** - Look around
- **U** - Toggle cursor lock/unlock
- **E** - Pick up items
- **[ ]** - Adjust FOV
