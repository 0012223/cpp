# Makefile for the ћ++ compiler
# Automatically detects target architecture and configures build options accordingly

# Compiler and flags
CC = gcc
CFLAGS_COMMON = -Wall -Wextra -std=c11 -I./src
CFLAGS_DEBUG = $(CFLAGS_COMMON) -g -O0 -DDEBUG
CFLAGS_RELEASE = $(CFLAGS_COMMON) -O3 -DNDEBUG

# Architecture detection
ARCH := $(shell uname -m)
ifeq ($(ARCH),x86_64)
    ARCH_FLAG = -DARCH_X86_64
    WORD_SIZE = 64
else ifeq ($(ARCH),i386)
    ARCH_FLAG = -DARCH_X86
    WORD_SIZE = 32
else ifeq ($(ARCH),i686)
    ARCH_FLAG = -DARCH_X86
    WORD_SIZE = 32
else
    $(warning Architecture $(ARCH) not explicitly supported, defaulting to x86_64)
    ARCH_FLAG = -DARCH_X86_64
    WORD_SIZE = 64
endif

# Define directories
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(SRC_DIR)/lexer/*.c) \
            $(wildcard $(SRC_DIR)/parser/*.c) \
            $(wildcard $(SRC_DIR)/semantic/*.c) \
            $(wildcard $(SRC_DIR)/ir/*.c) \
            $(wildcard $(SRC_DIR)/codegen/*.c) \
            $(wildcard $(SRC_DIR)/target/*.c) \
            $(wildcard $(SRC_DIR)/utils/*.c)

# Test files
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)

# Object files
OBJ_FILES_DEBUG = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/debug/%.o,$(SRC_FILES))
OBJ_FILES_RELEASE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/release/%.o,$(SRC_FILES))
TEST_OBJ_FILES = $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/test/%.o,$(TEST_FILES))

# Output binary
TARGET = ћпп

# Default target
all: release

# Create build directories
$(BUILD_DIR)/debug/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -c $< -o $@

$(BUILD_DIR)/release/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_RELEASE) $(ARCH_FLAG) -c $< -o $@

$(BUILD_DIR)/test/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -c $< -o $@

# Debug build
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: $(OBJ_FILES_DEBUG)
	@echo "Building debug version ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o $(TARGET)_debug $^

# Release build
release: CFLAGS = $(CFLAGS_RELEASE)
release: $(OBJ_FILES_RELEASE)
	@echo "Building release version ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_RELEASE) $(ARCH_FLAG) -o $(TARGET) $^

# Test build
test: CFLAGS = $(CFLAGS_DEBUG)
test: $(OBJ_FILES_DEBUG) $(TEST_OBJ_FILES)
	@echo "Building and running tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o $(TARGET)_test $^
	./$(TARGET)_test

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TARGET)_debug $(TARGET)_test

# Install
install: release
	@echo "Installing ћпп compiler to /usr/local/bin/..."
	cp $(TARGET) /usr/local/bin/

# Help
help:
	@echo "ћ++ Compiler Makefile"
	@echo "---------------------"
	@echo "Targets:"
	@echo "  all:       Alias for 'release'"
	@echo "  debug:     Build with debug symbols and no optimization"
	@echo "  release:   Build optimized release version"
	@echo "  test:      Build and run tests"
	@echo "  clean:     Remove build files"
	@echo "  install:   Install the compiler to /usr/local/bin/"
	@echo "  help:      Show this help message"
	@echo ""
	@echo "Architecture: $(ARCH) ($(WORD_SIZE)-bit word size)"

.PHONY: all debug release test clean install help