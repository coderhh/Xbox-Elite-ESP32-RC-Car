# Xbox Elite ESP32 RC Car - Development Makefile

# Default target
.DEFAULT_GOAL := help

# Variables
PLATFORM_NATIVE := native
PLATFORM_ESP32 := esp32dev
PLATFORM_ESP32_TEST := esp32dev_test
PORT := /dev/cu.usbserial*

# Colors for output
RED := \033[31m
GREEN := \033[32m
YELLOW := \033[33m
BLUE := \033[34m
RESET := \033[0m

##@ Development Commands

.PHONY: help
help: ## Display this help message
	@echo "$(BLUE)Xbox Elite ESP32 RC Car - Development Commands$(RESET)"
	@echo ""
	@awk 'BEGIN {FS = ":.*##"; printf "Usage: make $(GREEN)<command>$(RESET)\n\nCommands:\n"} /^[a-zA-Z_-]+:.*?##/ { printf "  $(GREEN)%-15s$(RESET) %s\n", $$1, $$2 } /^##@/ { printf "\n$(YELLOW)%s$(RESET)\n", substr($$0, 5) } ' $(MAKEFILE_LIST)

##@ Testing

.PHONY: test
test: ## Run all tests on native platform
	@echo "$(BLUE)Running all tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -v

.PHONY: test-unit
test-unit: ## Run specific unit tests
	@echo "$(BLUE)Running unit tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -f test_motor_controller -f test_controller_processor -f test_safety_system

.PHONY: test-integration
test-integration: ## Run integration tests
	@echo "$(BLUE)Running integration tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -f test_integration

.PHONY: test-motor
test-motor: ## Run motor controller tests
	@echo "$(BLUE)Running motor controller tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -f test_motor_controller

.PHONY: test-controller
test-controller: ## Run controller processor tests
	@echo "$(BLUE)Running controller processor tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -f test_controller_processor

.PHONY: test-safety
test-safety: ## Run safety system tests
	@echo "$(BLUE)Running safety system tests...$(RESET)"
	pio test -e $(PLATFORM_NATIVE) -f test_safety_system

.PHONY: test-esp32
test-esp32: ## Run tests on ESP32 hardware
	@echo "$(BLUE)Running tests on ESP32 hardware...$(RESET)"
	pio test -e $(PLATFORM_ESP32_TEST) --upload-port $(PORT)

.PHONY: test-watch
test-watch: ## Watch for file changes and run tests automatically
	@echo "$(BLUE)Watching for changes and running tests...$(RESET)"
	@echo "$(YELLOW)Press Ctrl+C to stop$(RESET)"
	@while true; do \
		make test; \
		echo "$(GREEN)Tests completed. Waiting for changes...$(RESET)"; \
		sleep 2; \
	done

##@ Building

.PHONY: build
build: ## Build for ESP32
	@echo "$(BLUE)Building for ESP32...$(RESET)"
	pio run -e $(PLATFORM_ESP32)

.PHONY: build-verbose
build-verbose: ## Build for ESP32 with verbose output
	@echo "$(BLUE)Building for ESP32 (verbose)...$(RESET)"
	pio run -e $(PLATFORM_ESP32) -v

.PHONY: upload
upload: ## Upload to ESP32
	@echo "$(BLUE)Uploading to ESP32...$(RESET)"
	pio run -e $(PLATFORM_ESP32) -t upload --upload-port $(PORT)

.PHONY: monitor
monitor: ## Open serial monitor
	@echo "$(BLUE)Opening serial monitor...$(RESET)"
	pio device monitor --port $(PORT) --baud 115200

.PHONY: upload-monitor
upload-monitor: upload monitor ## Upload and start monitoring

##@ Code Quality

.PHONY: check
check: ## Run static code analysis
	@echo "$(BLUE)Running static code analysis...$(RESET)"
	pio check --skip-packages

.PHONY: check-verbose
check-verbose: ## Run static code analysis with verbose output
	@echo "$(BLUE)Running static code analysis (verbose)...$(RESET)"
	pio check --skip-packages -v

.PHONY: format
format: ## Format code (if formatter is available)
	@echo "$(BLUE)Formatting code...$(RESET)"
	@echo "$(YELLOW)Note: Install clang-format for automatic code formatting$(RESET)"
	@command -v clang-format >/dev/null 2>&1 && \
		find src include -name "*.cpp" -o -name "*.h" | xargs clang-format -i || \
		echo "$(RED)clang-format not found. Install it for code formatting.$(RESET)"

##@ Maintenance

.PHONY: clean
clean: ## Clean build artifacts
	@echo "$(BLUE)Cleaning build artifacts...$(RESET)"
	pio run -t clean

.PHONY: install-deps
install-deps: ## Install/update project dependencies
	@echo "$(BLUE)Installing project dependencies...$(RESET)"
	pio pkg install

.PHONY: update-deps
update-deps: ## Update project dependencies
	@echo "$(BLUE)Updating project dependencies...$(RESET)"
	pio pkg update

.PHONY: list-devices
list-devices: ## List available serial devices
	@echo "$(BLUE)Available serial devices:$(RESET)"
	pio device list

##@ Documentation

.PHONY: docs
docs: ## Generate documentation
	@echo "$(BLUE)Generating documentation...$(RESET)"
	@echo "$(YELLOW)Documentation available in:$(RESET)"
	@echo "  - README.md: Main project documentation"
	@echo "  - WIRING.md: Hardware wiring guide"
	@echo "  - TEST.md: Testing documentation"

##@ Development Workflow

.PHONY: dev-setup
dev-setup: ## Set up development environment
	@echo "$(BLUE)Setting up development environment...$(RESET)"
	@echo "$(YELLOW)1. Installing dependencies...$(RESET)"
	make install-deps
	@echo "$(YELLOW)2. Running initial tests...$(RESET)"
	make test
	@echo "$(YELLOW)3. Building project...$(RESET)"
	make build
	@echo "$(GREEN)Development environment ready!$(RESET)"

.PHONY: pre-commit
pre-commit: ## Run pre-commit checks
	@echo "$(BLUE)Running pre-commit checks...$(RESET)"
	@echo "$(YELLOW)1. Running tests...$(RESET)"
	make test || (echo "$(RED)Tests failed!$(RESET)" && exit 1)
	@echo "$(YELLOW)2. Running static analysis...$(RESET)"
	make check || (echo "$(RED)Static analysis failed!$(RESET)" && exit 1)
	@echo "$(YELLOW)3. Building project...$(RESET)"
	make build || (echo "$(RED)Build failed!$(RESET)" && exit 1)
	@echo "$(GREEN)All pre-commit checks passed!$(RESET)"

.PHONY: ci
ci: ## Run full CI pipeline locally
	@echo "$(BLUE)Running full CI pipeline...$(RESET)"
	make clean
	make install-deps
	make test
	make build
	make check
	@echo "$(GREEN)CI pipeline completed successfully!$(RESET)"

##@ Hardware Testing

.PHONY: hardware-test
hardware-test: ## Upload and run comprehensive hardware test
	@echo "$(BLUE)Running comprehensive hardware test...$(RESET)"
	make upload-monitor

.PHONY: debug-hardware
debug-hardware: ## Debug hardware issues
	@echo "$(BLUE)Hardware debugging mode...$(RESET)"
	@echo "$(YELLOW)1. Checking device connection...$(RESET)"
	make list-devices
	@echo "$(YELLOW)2. Uploading debug build...$(RESET)"
	make upload
	@echo "$(YELLOW)3. Starting monitor...$(RESET)"
	make monitor

##@ Shortcuts

.PHONY: t
t: test ## Shortcut for 'make test'

.PHONY: b
b: build ## Shortcut for 'make build'

.PHONY: u
u: upload ## Shortcut for 'make upload'

.PHONY: um
um: upload-monitor ## Shortcut for 'make upload-monitor'

.PHONY: c
c: clean ## Shortcut for 'make clean'