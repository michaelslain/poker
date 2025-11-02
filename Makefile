# Use C++ compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I/opt/homebrew/opt/raylib/include -I/opt/homebrew/opt/ode/include -Iinclude
LDFLAGS = -L/opt/homebrew/opt/raylib/lib -L/opt/homebrew/opt/ode/lib -lraylib -lode -lm

# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows
    LDFLAGS += -lopengl32 -lgdi32 -lwinmm
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux
        LDFLAGS += -lGL -lpthread -ldl -lrt -lX11
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS
        LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo
    endif
endif

# Target executable
TARGET = game

# Source files (C++ extensions)
SRCS = main.cpp src/object.cpp src/camera.cpp src/player.cpp src/interactable.cpp src/item.cpp src/card.cpp src/chip.cpp src/deck.cpp src/poker_table.cpp src/dom.cpp src/inventory.cpp src/inventory_ui.cpp src/render_utils.cpp src/plane.cpp src/physics.cpp src/rigidbody.cpp src/spawner.cpp
OBJS = $(SRCS:.cpp=.o)

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET) src/*.o

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
