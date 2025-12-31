# Makefile for Unix Shell Project
# Operating Systems Course - Shell Implementation with Parallel Projects

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O2 -DNDEBUG
PTHREAD_FLAGS = -pthread
MATH_FLAGS = -lm

# Directories
SHELL_DIR = shell
SUDOKU_DIR = sudoku
MONTE_CARLO_DIR = monte_carlo
BUILD_DIR = build

# Source files
SHELL_SRC = $(SHELL_DIR)/main.c
SUDOKU_SRC = $(SUDOKU_DIR)/sudoku_validator.c
MONTE_CARLO_SRC = $(MONTE_CARLO_DIR)/monte_carlo.c

# Executable names
SHELL_BIN = $(BUILD_DIR)/unixsh
SUDOKU_BIN = $(BUILD_DIR)/sudoku_validator
MONTE_CARLO_BIN = $(BUILD_DIR)/monte_carlo

# Default target - build all projects
.PHONY: all
all: $(BUILD_DIR) $(SHELL_BIN) $(SUDOKU_BIN) $(MONTE_CARLO_BIN)
	@echo "✓ All projects built successfully!"
	@echo ""
	@echo "Executables created:"
	@echo "  - Unix Shell:         $(SHELL_BIN)"
	@echo "  - Sudoku Validator:   $(SUDOKU_BIN)"
	@echo "  - Monte Carlo Pi:     $(MONTE_CARLO_BIN)"
	@echo ""
	@echo "Run 'make test' to test all components"
	@echo "Run 'make demo' for a comprehensive demonstration"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "Created build directory: $(BUILD_DIR)"

# Build Unix Shell (mandatory component - 60 points)
$(SHELL_BIN): $(SHELL_SRC) | $(BUILD_DIR)
	@echo "Building Unix Shell..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o $@ $<
	@echo "✓ Unix Shell built: $@"

# Build Sudoku Validator (optional component - 20 points)
$(SUDOKU_BIN): $(SUDOKU_SRC) | $(BUILD_DIR)
	@echo "Building Sudoku Validator..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(PTHREAD_FLAGS) -o $@ $<
	@echo "✓ Sudoku Validator built: $@"

# Build Monte Carlo Pi Estimation (optional component - 20 points)
$(MONTE_CARLO_BIN): $(MONTE_CARLO_SRC) | $(BUILD_DIR)
	@echo "Building Monte Carlo Pi Estimation..."
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(MATH_FLAGS) -o $@ $<
	@echo "✓ Monte Carlo Pi Estimation built: $@"

# Debug builds
.PHONY: debug debug-shell debug-sudoku debug-monte-carlo
debug: debug-shell debug-sudoku debug-monte-carlo

debug-shell: $(BUILD_DIR)
	@echo "Building Unix Shell (debug)..."
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(BUILD_DIR)/unixsh_debug $(SHELL_SRC)
	@echo "✓ Debug Unix Shell built: $(BUILD_DIR)/unixsh_debug"

debug-sudoku: $(BUILD_DIR)
	@echo "Building Sudoku Validator (debug)..."
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(PTHREAD_FLAGS) -o $(BUILD_DIR)/sudoku_validator_debug $(SUDOKU_SRC)
	@echo "✓ Debug Sudoku Validator built: $(BUILD_DIR)/sudoku_validator_debug"

debug-monte-carlo: $(BUILD_DIR)
	@echo "Building Monte Carlo Pi Estimation (debug)..."
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $(MATH_FLAGS) -o $(BUILD_DIR)/monte_carlo_debug $(MONTE_CARLO_SRC)
	@echo "✓ Debug Monte Carlo Pi Estimation built: $(BUILD_DIR)/monte_carlo_debug"

# Individual component builds
.PHONY: shell sudoku monte-carlo
shell: $(SHELL_BIN)
sudoku: $(SUDOKU_BIN)
monte-carlo: $(MONTE_CARLO_BIN)

# Test targets
.PHONY: test test-shell test-sudoku test-monte-carlo
test: test-sudoku test-monte-carlo
	@echo ""
	@echo "=== ALL TESTS COMPLETED ==="
	@echo "✓ Sudoku Validator: Tested with valid and invalid puzzles"
	@echo "✓ Monte Carlo Pi:   Tested with parallel computation"
	@echo ""
	@echo "To test the shell interactively, run: $(SHELL_BIN)"

test-shell: $(SHELL_BIN)
	@echo "=== Testing Unix Shell ==="
	@echo "Note: Shell testing requires interactive usage"
	@echo "Run the following command to test the shell:"
	@echo "  $(SHELL_BIN)"
	@echo ""
	@echo "Test these features:"
	@echo "  1. Basic commands: ls, pwd, whoami"
	@echo "  2. Built-in commands: cd, pwd, help, history"
	@echo "  3. Background execution: sleep 3 &"
	@echo "  4. Pipes: ls -la | grep txt"
	@echo "  5. History: !!"
	@echo "  6. Exit: exit"

test-sudoku: $(SUDOKU_BIN)
	@echo "=== Testing Sudoku Validator ==="
	@echo "Testing with valid Sudoku puzzle..."
	$(SUDOKU_BIN) $(SUDOKU_DIR)/valid_sudoku.txt
	@echo ""
	@echo "Testing with invalid Sudoku puzzle..."
	$(SUDOKU_BIN) $(SUDOKU_DIR)/invalid_sudoku.txt
	@echo "✓ Sudoku validator test completed"

test-monte-carlo: $(MONTE_CARLO_BIN)
	@echo "=== Testing Monte Carlo Pi Estimation ==="
	@echo "Running quick test with 2 processes and 1M points..."
	$(MONTE_CARLO_BIN) 2 1000000
	@echo "✓ Monte Carlo test completed"

# Comprehensive demonstration
.PHONY: demo
demo: all
	@echo ""
	@echo "========================================="
	@echo "   OPERATING SYSTEMS SHELL PROJECT"
	@echo "           DEMONSTRATION"
	@echo "========================================="
	@echo ""
	@echo "Project Components:"
	@echo "  1. Unix Shell Implementation (60 points)"
	@echo "  2. Parallel Sudoku Validator (20 points)"
	@echo "  3. Monte Carlo Pi Estimation (20 points)"
	@echo "  Total: 100 points"
	@echo ""
	@echo "Testing Sudoku Validator..."
	@$(MAKE) test-sudoku --no-print-directory
	@echo ""
	@echo "Testing Monte Carlo Pi Estimation..."
	@$(MAKE) test-monte-carlo --no-print-directory
	@echo ""
	@echo "========================================="
	@echo "Shell is ready for interactive testing:"
	@echo "  $(SHELL_BIN)"
	@echo "========================================="

# Performance benchmarks
.PHONY: benchmark benchmark-sudoku benchmark-monte-carlo
benchmark: benchmark-sudoku benchmark-monte-carlo

benchmark-sudoku: $(SUDOKU_BIN)
	@echo "=== Sudoku Validator Performance Benchmark ==="
	@echo "Running multiple validations to measure performance..."
	@time $(SUDOKU_BIN) $(SUDOKU_DIR)/valid_sudoku.txt > /dev/null
	@echo "✓ Sudoku performance benchmark completed"

benchmark-monte-carlo: $(MONTE_CARLO_BIN)
	@echo "=== Monte Carlo Performance Benchmark ==="
	@echo "Testing different process counts and point numbers..."
	@echo "1 process, 10M points:"
	@time $(MONTE_CARLO_BIN) 1 10000000 | grep "Estimated"
	@echo "2 processes, 10M points:"
	@time $(MONTE_CARLO_BIN) 2 10000000 | grep "Estimated"
	@echo "4 processes, 10M points:"
	@time $(MONTE_CARLO_BIN) 4 10000000 | grep "Estimated"
	@echo "✓ Monte Carlo performance benchmark completed"

# Installation and setup
.PHONY: install
install: all
	@echo "Installing executables to /usr/local/bin (requires sudo)..."
	sudo cp $(SHELL_BIN) /usr/local/bin/unixsh
	sudo cp $(SUDOKU_BIN) /usr/local/bin/sudoku_validator
	sudo cp $(MONTE_CARLO_BIN) /usr/local/bin/monte_carlo
	@echo "✓ Installation completed"
	@echo "You can now run:"
	@echo "  unixsh"
	@echo "  sudoku_validator"
	@echo "  monte_carlo"

# Clean targets
.PHONY: clean clean-build clean-all
clean: clean-build

clean-build:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	@echo "✓ Build directory cleaned"

clean-all: clean-build
	@echo "Performing deep clean..."
	find . -name "*.o" -delete
	find . -name "*.a" -delete
	find . -name "*.so" -delete
	find . -name "*~" -delete
	find . -name ".DS_Store" -delete
	@echo "✓ Deep clean completed"

# Documentation and project info
.PHONY: info help docs
info:
	@echo "========================================="
	@echo "      UNIX SHELL PROJECT INFO"
	@echo "========================================="
	@echo "Course: Operating Systems"
	@echo "Project: Unix Shell with Parallel Computing"
	@echo ""
	@echo "Components:"
	@echo "  1. Unix Shell (mandatory)     - 60 points"
	@echo "     Features: command parsing, built-ins,"
	@echo "     pipes, history, background execution"
	@echo ""
	@echo "  2. Sudoku Validator (optional) - 20 points"
	@echo "     Parallel validation using pthreads"
	@echo ""
	@echo "  3. Monte Carlo Pi (optional)   - 20 points"
	@echo "     Parallel Pi estimation using processes"
	@echo ""
	@echo "Total Score: 100 points"
	@echo "========================================="

help:
	@echo "Available targets:"
	@echo "  all           - Build all components (default)"
	@echo "  shell         - Build only Unix shell"
	@echo "  sudoku        - Build only Sudoku validator"
	@echo "  monte-carlo   - Build only Monte Carlo Pi estimation"
	@echo "  debug         - Build all components with debug info"
	@echo "  test          - Run all tests"
	@echo "  demo          - Run comprehensive demonstration"
	@echo "  benchmark     - Run performance benchmarks"
	@echo "  install       - Install executables system-wide"
	@echo "  clean         - Remove build artifacts"
	@echo "  info          - Show project information"
	@echo "  help          - Show this help message"

docs:
	@echo "Generating documentation..."
	@echo "Project documentation is available in README.md"
	@echo "Key files:"
	@echo "  README.md              - Complete project documentation"
	@echo "  shell/main.c           - Unix shell implementation"
	@echo "  sudoku/sudoku_validator.c - Parallel Sudoku validator"
	@echo "  monte_carlo/monte_carlo.c - Monte Carlo Pi estimation"
	@echo "  Makefile               - Build system"

# Check code quality
.PHONY: check lint
check: lint

lint:
	@echo "Checking code quality..."
	@echo "Checking shell implementation..."
	@gcc -Wall -Wextra -std=c99 -pedantic -fsyntax-only $(SHELL_SRC) && echo "✓ Shell code OK"
	@echo "Checking Sudoku validator..."
	@gcc -Wall -Wextra -std=c99 -pedantic -pthread -fsyntax-only $(SUDOKU_SRC) && echo "✓ Sudoku code OK"
	@echo "Checking Monte Carlo implementation..."
	@gcc -Wall -Wextra -std=c99 -pedantic -lm -fsyntax-only $(MONTE_CARLO_SRC) && echo "✓ Monte Carlo code OK"
	@echo "✓ All code passed syntax check"

# Package for submission
.PHONY: package
package: clean all
	@echo "Creating submission package..."
	tar czf shell-project-submission.tar.gz \
		README.md LICENSE Makefile \
		$(SHELL_DIR)/ $(SUDOKU_DIR)/ $(MONTE_CARLO_DIR)/ \
		$(BUILD_DIR)/
	@echo "✓ Package created: shell-project-submission.tar.gz"

.PHONY: zip
zip: clean all
	@echo "Creating submission ZIP..."
	zip -r shell-project-submission.zip \
		README.md LICENSE Makefile \
		$(SHELL_DIR)/ $(SUDOKU_DIR)/ $(MONTE_CARLO_DIR)/ \
		$(BUILD_DIR)/
	@echo "✓ ZIP package created: shell-project-submission.zip"

# Quick run shortcuts
.PHONY: run-shell run-sudoku run-monte-carlo
run-shell: $(SHELL_BIN)
	$(SHELL_BIN)

run-sudoku: $(SUDOKU_BIN)
	$(SUDOKU_BIN) $(SUDOKU_DIR)/valid_sudoku.txt

run-monte-carlo: $(MONTE_CARLO_BIN)
	$(MONTE_CARLO_BIN) 4 10000000