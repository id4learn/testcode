CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O2 -g
INCLUDES = -I.

# Source files
SOURCES = tlv_parser.cpp
HEADERS = tlv_parser.hpp
OBJECTS = $(SOURCES:.cpp=.o)

# Target executables
EXAMPLE = example_usage
TEST_TARGET = simple_test

# Default target
all: $(EXAMPLE)

# Build the example
$(EXAMPLE): $(OBJECTS) example_usage.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build the simple test
$(TEST_TARGET): $(OBJECTS) simple_test.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Clean build artifacts
clean:
	rm -f $(OBJECTS) example_usage.o simple_test.o $(EXAMPLE) $(TEST_TARGET)

# Run the example
run: $(EXAMPLE)
	./$(EXAMPLE)

# Run the simple test
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Debug build
debug: CXXFLAGS += -DDEBUG -O0
debug: clean $(EXAMPLE)

# Release build with optimizations
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(EXAMPLE)

# Check for memory leaks (requires valgrind)
valgrind: $(EXAMPLE)
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXAMPLE)

# Format code (requires clang-format)
format:
	clang-format -i *.cpp *.hpp

# Static analysis (requires cppcheck)
analyze:
	cppcheck --enable=all --std=c++20 $(SOURCES) $(HEADERS)

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the example (default)"
	@echo "  test     - Build and run the simple test"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the example"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build with optimization flags"
	@echo "  valgrind - Run with memory leak detection"
	@echo "  format   - Format code with clang-format"
	@echo "  analyze  - Run static analysis with cppcheck"
	@echo "  help     - Show this help message"

.PHONY: all clean run debug release valgrind format analyze help