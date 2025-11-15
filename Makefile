# Use C++ compiler
CXX = g++
# Use ccache for faster compilation
CXX = ccache g++

# Enable parallel compilation (use all CPU cores)
MAKEFLAGS += -j$(shell sysctl -n hw.ncpu)

# Base compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -I/opt/homebrew/opt/raylib/include -I/opt/homebrew/opt/ode/include -Isrc
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

# Source files (C++ extensions) - automatically find all .cpp files in src/
SRCS = main.cpp $(shell find src -name '*.cpp')
OBJS = $(SRCS:.cpp=.o)

# Test files
TEST_SRCS = tests/catch_amalgamated.cpp tests/test_main.cpp tests/test_object.cpp tests/test_camera.cpp tests/test_card.cpp tests/test_chip.cpp tests/test_chip_stack.cpp tests/test_deck.cpp tests/test_inventory.cpp tests/test_dom.cpp tests/test_interactable.cpp tests/test_item.cpp tests/test_weapon.cpp tests/test_substance.cpp tests/test_person.cpp tests/test_enemy.cpp tests/test_dealer.cpp tests/test_spawner.cpp tests/test_floor.cpp tests/test_ceiling.cpp tests/test_wall.cpp tests/test_light.cpp tests/test_lighting_manager.cpp tests/test_light_bulb.cpp tests/test_physics.cpp tests/test_rigidbody.cpp tests/test_pistol.cpp tests/test_poker_table.cpp tests/test_inventory_ui.cpp tests/test_render_utils.cpp tests/test_insanity_manager.cpp tests/test_death_scene.cpp
# Objects needed for tests (everything except main.cpp) - automatically find all .o files in src/
TEST_OBJS = $(shell find src -name '*.cpp' | sed 's/\.cpp/\.o/')
TEST_ALL_OBJS = $(TEST_SRCS:.cpp=.o) $(TEST_OBJS)

# Build targets
all: release

# Debug build (fast compilation, slower runtime, better error messages)
debug: CXXFLAGS += -O0 -g -DDEBUG
debug: $(TARGET)
	@echo "✓ Debug build complete - use './game' to run"

# Release build (slower compilation, faster runtime, optimized)
release: CXXFLAGS += -O2
release: $(TARGET)
	@echo "✓ Release build complete - use './game' to run"

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Build and run tests (always in debug mode for better error messages)
test: CXXFLAGS += -O0 -g -DDEBUG
test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_ALL_OBJS)
	@echo "Linking $(TEST_TARGET)..."
	@$(CXX) $(TEST_ALL_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

# Clean only test artifacts
clean-test:
	rm -f tests/*.o $(TEST_TARGET)

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

# Run the game (builds in release mode by default)
run: release
	./$(TARGET)

# Run in debug mode (faster compilation)
run-debug: debug
	./$(TARGET)

# Show ccache statistics
ccache-stats:
	@ccache -s

# Clear ccache
ccache-clear:
	@ccache -C
	@echo "✓ ccache cleared"

.PHONY: all debug release clean run run-debug test ccache-stats ccache-clear
