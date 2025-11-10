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
TEST_TARGET = test_runner

# Source files (C++ extensions)
SRCS = main.cpp src/object.cpp src/person.cpp src/enemy.cpp src/dealer.cpp src/camera.cpp src/player.cpp src/interactable.cpp src/item.cpp src/card.cpp src/chip.cpp src/chip_stack.cpp src/pistol.cpp src/deck.cpp src/poker_table.cpp src/dom.cpp src/inventory.cpp src/inventory_ui.cpp src/render_utils.cpp src/floor.cpp src/ceiling.cpp src/wall.cpp src/light.cpp src/light_bulb.cpp src/collider.cpp src/physics.cpp src/rigidbody.cpp src/spawner.cpp src/scene.cpp src/scene_manager.cpp scenes/game_scene.cpp
OBJS = $(SRCS:.cpp=.o)

# Test files
TEST_SRCS = tests/catch_amalgamated.cpp tests/test_main.cpp tests/test_object.cpp tests/test_camera.cpp tests/test_card.cpp tests/test_chip.cpp tests/test_chip_stack.cpp tests/test_deck.cpp tests/test_inventory.cpp tests/test_dom.cpp tests/test_interactable.cpp tests/test_item.cpp tests/test_person.cpp tests/test_enemy.cpp tests/test_dealer.cpp tests/test_spawner.cpp tests/test_floor.cpp tests/test_ceiling.cpp tests/test_wall.cpp tests/test_light_bulb.cpp tests/test_physics.cpp tests/test_rigidbody.cpp tests/test_pistol.cpp tests/test_poker_table.cpp tests/test_inventory_ui.cpp tests/test_render_utils.cpp
# Objects needed for tests (everything except main.cpp)
TEST_OBJS = src/object.o src/person.o src/enemy.o src/dealer.o src/camera.o src/player.o src/interactable.o src/item.o src/card.o src/chip.o src/chip_stack.o src/pistol.o src/deck.o src/poker_table.o src/dom.o src/inventory.o src/inventory_ui.o src/render_utils.o src/floor.o src/ceiling.o src/wall.o src/light.o src/light_bulb.o src/collider.o src/physics.o src/rigidbody.o src/spawner.o src/scene.o src/scene_manager.o scenes/game_scene.o
TEST_ALL_OBJS = $(TEST_SRCS:.cpp=.o) $(TEST_OBJS)

# Build target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Build and run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_ALL_OBJS)
	$(CXX) $(TEST_ALL_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

# Test file compilation (must come before generic %.o rule)
tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -Itests -DCATCH_AMALGAMATED_CUSTOM_MAIN -c $< -o $@

# Source file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Scenes directory compilation
scenes/%.o: scenes/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET) src/*.o tests/*.o scenes/*.o $(TEST_TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run test
