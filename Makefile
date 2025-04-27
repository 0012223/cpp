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

# UTF-8 Test files
UTF8_TEST_SRC = $(TEST_DIR)/utf8_tests.c
UTF8_TEST_OBJ = $(BUILD_DIR)/test/utf8_tests.o
UTF8_UTILS_SRC = $(SRC_DIR)/utils/utf8.c
UTF8_UTILS_OBJ = $(BUILD_DIR)/debug/utils/utf8.o

# Error Test files
ERROR_TEST_SRC = $(TEST_DIR)/error_tests.c
ERROR_TEST_OBJ = $(BUILD_DIR)/test/error_tests.o
ERROR_UTILS_SRC = $(SRC_DIR)/utils/error.c
ERROR_UTILS_OBJ = $(BUILD_DIR)/debug/utils/error.o

# AST Test files
AST_TEST_SRC = $(TEST_DIR)/ast_tests.c
AST_TEST_OBJ = $(BUILD_DIR)/test/ast_tests.o

# Parser Test files
PARSER_TEST_SRC = $(TEST_DIR)/parser_tests.c
PARSER_TEST_OBJ = $(BUILD_DIR)/test/parser_tests.o

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

# UTF-8 Test build
test-utf8: CFLAGS = $(CFLAGS_DEBUG)
test-utf8: $(BUILD_DIR)/debug/utils/utf8.o $(BUILD_DIR)/test/utf8_tests.o
	@echo "Building and running UTF-8 tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o utf8_test $^
	./utf8_test

# Error handling Test build
test-error: CFLAGS = $(CFLAGS_DEBUG)
test-error: $(BUILD_DIR)/debug/utils/error.o $(BUILD_DIR)/test/error_tests.o
	@echo "Building and running error handling tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o error_test $^
	./error_test

# Lexer Test files
LEXER_TEST_SRC = $(TEST_DIR)/lexer_tests.c
LEXER_TEST_OBJ = $(BUILD_DIR)/test/lexer_tests.o
LEXER_SRC = $(SRC_DIR)/lexer/keywords.c
LEXER_OBJ = $(BUILD_DIR)/debug/lexer/keywords.o

# Lexer Test build
test-lexer: CFLAGS = $(CFLAGS_DEBUG)
test-lexer: $(LEXER_OBJ) $(LEXER_TEST_OBJ) $(BUILD_DIR)/debug/utils/utf8.o $(BUILD_DIR)/debug/utils/error.o $(BUILD_DIR)/debug/target/target_info.o $(BUILD_DIR)/debug/lexer/lexer.o
	@echo "Building and running lexer tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o lexer_test $^
	./lexer_test

# AST Test build
test-ast: CFLAGS = $(CFLAGS_DEBUG)
test-ast: $(AST_TEST_OBJ) $(BUILD_DIR)/debug/parser/ast.o $(BUILD_DIR)/debug/utils/utf8.o $(BUILD_DIR)/debug/utils/error.o $(BUILD_DIR)/debug/target/target_info.o $(BUILD_DIR)/debug/lexer/lexer.o $(BUILD_DIR)/debug/lexer/keywords.o
	@echo "Building and running AST tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o ast_test $^
	./ast_test

# Parser Test build
test-parser: CFLAGS = $(CFLAGS_DEBUG)
test-parser: $(PARSER_TEST_OBJ) $(BUILD_DIR)/debug/parser/ast.o $(BUILD_DIR)/debug/parser/parser.o $(BUILD_DIR)/debug/utils/utf8.o $(BUILD_DIR)/debug/utils/error.o $(BUILD_DIR)/debug/target/target_info.o $(BUILD_DIR)/debug/lexer/lexer.o $(BUILD_DIR)/debug/lexer/keywords.o
	@echo "Building and running parser tests ($(WORD_SIZE)-bit)..."
	$(CC) $(CFLAGS_DEBUG) $(ARCH_FLAG) -o parser_test $^
	./parser_test

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET) $(TARGET)_debug $(TARGET)_test utf8_test error_test lexer_test ast_test parser_test
	rm -f ћпп_error_log_*.txt

# Install
install: release
	@echo "Installing ћпп compiler to /usr/local/bin/..."
	cp $(TARGET) /usr/local/bin/

# Help
help:
	@echo "ћ++ Compiler Makefile"
	@echo "---------------------"
	@echo "Targets:"
	@echo "  all:        Alias for 'release'"
	@echo "  debug:      Build with debug symbols and no optimization"
	@echo "  release:    Build optimized release version"
	@echo "  test:       Build and run tests"
	@echo "  test-utf8:  Build and run only UTF-8 tests"
	@echo "  test-error: Build and run only error handling tests"
	@echo "  test-lexer: Build and run only lexer tests"
	@echo "  test-ast:   Build and run only AST tests"
	@echo "  test-parser: Build and run only parser tests"
	@echo "  clean:      Remove build files"
	@echo "  install:    Install the compiler to /usr/local/bin/"
	@echo "  help:       Show this help message"
	@echo ""
	@echo "Architecture: $(ARCH) ($(WORD_SIZE)-bit word size)"

.PHONY: all debug release test test-utf8 test-error test-lexer test-ast test-parser clean install help