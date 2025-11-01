# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I/opt/homebrew/opt/raylib/include -I/opt/homebrew/opt/ode/include -Iinclude
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

# Source files
SRCS = main.c src/object.c src/camera.c src/player.c src/interactable.c src/item.c src/card.c src/chip.c src/inventory.c src/inventory_ui.c src/render_utils.c src/plane.c src/physics.c src/rigidbody.c src/spawner.c
OBJS = $(SRCS:.c=.o)

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET) src/*.o

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
