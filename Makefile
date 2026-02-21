# Makefile for C Long Arithmetic Library
# Compiles with -O0 and all available sanitizers for thorough testing

CC = gcc
CFLAGS = -O0 -g -Wall -Wextra -Wpedantic -std=c11
SRC_DIR = src
BUILD_DIR = build

# Source files
SOURCES = $(SRC_DIR)/superlong.c $(SRC_DIR)/safe-alloc.c
HEADERS = $(SRC_DIR)/superlong.h $(SRC_DIR)/safe-alloc.h $(SRC_DIR)/generate-arr.h
TEST_SRC = test.c

# Object files
OBJECTS = $(BUILD_DIR)/superlong.o $(BUILD_DIR)/safe-alloc.o
TEST_OBJ = $(BUILD_DIR)/test.o

# Sanitizer flags
# AddressSanitizer: detects memory errors (buffer overflows, use-after-free, etc.)
# UndefinedBehaviorSanitizer: catches undefined behavior
# LeakSanitizer: finds memory leaks (included with AddressSanitizer)
SANITIZERS = -fsanitize=address -fsanitize=undefined -fsanitize=leak
SANITIZER_FLAGS = $(SANITIZERS) -fno-omit-frame-pointer

# Output executables
TEST_EXEC = $(BUILD_DIR)/test_program

.PHONY: all test clean directories

# Default target
all: directories $(TEST_EXEC)

# Create build directory
directories:
	@mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/superlong.o: $(SRC_DIR)/superlong.c $(HEADERS)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) -c $< -o $@

$(BUILD_DIR)/safe-alloc.o: $(SRC_DIR)/safe-alloc.c $(SRC_DIR)/safe-alloc.h
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) -c $< -o $@

# Compile test file
$(BUILD_DIR)/test.o: $(TEST_SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) -I$(SRC_DIR) -c $< -o $@

# Link test executable
$(TEST_EXEC): $(OBJECTS) $(TEST_OBJ)
	$(CC) $(CFLAGS) $(SANITIZER_FLAGS) $^ -o $@

# Run tests
test: $(TEST_EXEC)
	@echo "=========================================="
	@echo "Running tests with sanitizers enabled..."
	@echo "  - AddressSanitizer (memory errors)"
	@echo "  - UndefinedBehaviorSanitizer (UB)"
	@echo "  - LeakSanitizer (memory leaks)"
	@echo "=========================================="
	@ASAN_OPTIONS=detect_leaks=1:halt_on_error=0 \
	UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=0 \
	./$(TEST_EXEC)
	@echo "=========================================="
	@echo "All tests completed!"
	@echo "=========================================="

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory cleaned."

# Help target
help:
	@echo "C Long Arithmetic Library - Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  make          - Build the library and test program"
	@echo "  make test     - Build and run tests with sanitizers"
	@echo "  make clean    - Remove all build artifacts"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Compilation flags:"
	@echo "  -O0          : No optimization (better for debugging)"
	@echo "  -g           : Include debugging information"
	@echo "  -Wall -Wextra: Enable extensive warnings"
	@echo "  -fsanitize=address   : AddressSanitizer"
	@echo "  -fsanitize=undefined : UndefinedBehaviorSanitizer"
	@echo "  -fsanitize=leak      : LeakSanitizer"
