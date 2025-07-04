# TLV Parser Library

A production-quality, modern C++20 Tag-Length-Value (TLV) parser library designed for parsing binary data formats commonly used in networking protocols, configuration files, and data serialization.

## Features

- **Modern C++20**: Uses `std::span`, `std::optional`, and other modern C++ features
- **Memory Safe**: Zero-copy parsing with proper bounds checking
- **Configurable**: Support for different tag and length field sizes (1, 2, or 4 bytes)
- **Endianness Support**: Configurable big-endian, little-endian, or native byte order
- **Error Handling**: Comprehensive error handling with custom exception types
- **Iterator Interface**: Efficient iteration over TLV elements
- **Nested TLV Support**: Parse nested TLV structures
- **Builder Pattern**: Easy TLV data construction with fluent API
- **Production Ready**: Extensive validation, error handling, and documentation

## TLV Format

The TLV format consists of three components:
- **Tag**: Identifier for the data type (1, 2, or 4 bytes)
- **Length**: Size of the value field (1, 2, or 4 bytes)  
- **Value**: The actual data (variable length)

```
+--------+--------+----------------+
|  Tag   | Length |     Value      |
+--------+--------+----------------+
```

## Quick Start

### Basic Usage

```cpp
#include "tlv_parser.hpp"
using namespace tlv;

// Create TLV data
TlvBuilder builder;
builder.add(0x01, std::string("Hello World"))
       .add(0x02, static_cast<uint32_t>(12345));

// Parse TLV data
TlvParser parser;
auto elements = parser.parse_all(builder.data());

for (const auto& element : elements) {
    std::cout << "Tag: 0x" << std::hex << element.tag() 
              << ", Value: " << element.value_as_string() << std::endl;
}
```

### Configuration

```cpp
// Configure for 2-byte tags and lengths with little-endian byte order
TlvConfig config;
config.tag_size = 2;
config.length_size = 2;
config.endianness = Endianness::LittleEndian;

TlvParser parser(config);
TlvBuilder builder(config);
```

## API Reference

### Core Classes

#### `TlvElement`
Represents a single TLV element with methods to access tag, length, and value.

```cpp
uint32_t tag() const;                    // Get tag value
uint32_t length() const;                 // Get value length
std::span<const uint8_t> value() const;  // Get raw value data
std::string value_as_string() const;     // Get value as string
template<typename T> T value_as_integer() const; // Get value as integer
bool has_nested_tlv() const;             // Check for nested TLV data
```

#### `TlvParser`
Main parser class for reading TLV data.

```cpp
// Parse all elements
std::vector<TlvElement> parse_all(std::span<const uint8_t> data) const;

// Find elements by tag
std::optional<TlvElement> find_by_tag(std::span<const uint8_t> data, uint32_t tag) const;
std::vector<TlvElement> find_all_by_tag(std::span<const uint8_t> data, uint32_t tag) const;

// Create iterator
TlvIterator create_iterator(std::span<const uint8_t> data) const;

// Validate data
bool validate(std::span<const uint8_t> data) const;
```

#### `TlvBuilder`
Builder class for creating TLV data.

```cpp
// Add different types of data
TlvBuilder& add(uint32_t tag, std::span<const uint8_t> value);
TlvBuilder& add(uint32_t tag, const std::string& value);
template<typename T> TlvBuilder& add(uint32_t tag, T value);

// Get built data
const std::vector<uint8_t>& data() const;
```

#### `TlvIterator`
Iterator for efficient traversal of TLV elements.

```cpp
bool has_next() const;           // Check if more elements available
const TlvElement& current() const; // Get current element
TlvIterator& operator++();       // Move to next element
size_t position() const;         // Get current position
```

### Configuration

#### `TlvConfig`
Configuration structure for customizing TLV parsing behavior.

```cpp
struct TlvConfig {
    uint8_t tag_size = 1;           // Size of tag field (1, 2, or 4 bytes)
    uint8_t length_size = 1;        // Size of length field (1, 2, or 4 bytes)
    Endianness endianness = Endianness::BigEndian;
    bool strict_validation = true;   // Enable strict validation
};
```

#### `Endianness`
Enumeration for byte order configuration.

```cpp
enum class Endianness {
    BigEndian,    // Network byte order
    LittleEndian, // Intel byte order
    Native        // Platform native
};
```

## Examples

### Basic Parsing

```cpp
// Create some TLV data
TlvBuilder builder;
builder.add(0x01, std::string("username"))
       .add(0x02, static_cast<uint16_t>(1234))
       .add(0x03, std::string("payload data"));

// Parse the data
TlvParser parser;
auto elements = parser.parse_all(builder.data());

// Access parsed elements
for (const auto& element : elements) {
    switch (element.tag()) {
        case 0x01:
            std::cout << "Username: " << element.value_as_string() << std::endl;
            break;
        case 0x02:
            std::cout << "ID: " << element.value_as_integer<uint16_t>() << std::endl;
            break;
        case 0x03:
            std::cout << "Payload: " << element.value_as_string() << std::endl;
            break;
    }
}
```

### Iterator Usage

```cpp
TlvParser parser;
auto iterator = parser.create_iterator(data);

while (iterator.has_next()) {
    const auto& element = iterator.current();
    // Process element
    ++iterator;
}
```

### Nested TLV

```cpp
// Create nested TLV structure
TlvBuilder inner;
inner.add(0x01, std::string("inner value"));

TlvBuilder outer;
const auto& inner_data = inner.data();
outer.add(0x10, std::string("outer value"))
     .add(0x20, std::span<const uint8_t>(inner_data))  // Nested TLV
     .add(0x30, std::string("more data"));

// Parse with nesting
TlvParser parser;
auto elements = parser.parse_all(outer.data());

for (const auto& element : elements) {
    if (element.tag() == 0x20 && element.has_nested_tlv()) {
        // Parse nested TLV data
        auto nested = parser.parse_all(element.value());
        for (const auto& nested_element : nested) {
            // Process nested elements
        }
    }
}
```

### Error Handling

```cpp
try {
    TlvParser parser;
    auto elements = parser.parse_all(data);
    // Process elements
} catch (const TlvParseError& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}

// Or validate first
if (parser.validate(data)) {
    auto elements = parser.parse_all(data);
}
```

## Building

### Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Make (for Makefile build)

### Build Commands

```bash
# Build the example
make

# Run the example
make run

# Clean build artifacts
make clean

# Debug build
make debug

# Release build with optimizations
make release

# Memory leak check (requires valgrind)
make valgrind

# Show all available targets
make help
```

### Manual Compilation

```bash
g++ -std=c++20 -Wall -Wextra -O2 -o example tlv_parser.cpp example_usage.cpp
```

## Performance Considerations

- **Zero-copy parsing**: Uses `std::span` to avoid data copying
- **Efficient iteration**: Iterator pattern for memory-efficient traversal
- **Minimal allocations**: Only allocates when building element vectors
- **Bounds checking**: All memory accesses are bounds-checked
- **Cache-friendly**: Sequential memory access patterns

## Thread Safety

The TLV parser classes are **not thread-safe** by design for performance reasons. If you need to use the parser from multiple threads:

- Use separate parser instances per thread, or
- Add external synchronization (mutex, etc.)

The parsed `TlvElement` objects are safe to read from multiple threads as long as the underlying data buffer remains valid.

## License

This project is provided as example code. Feel free to use, modify, and distribute according to your needs.

## Contributing

Contributions are welcome! Please ensure your code:
- Follows the existing code style
- Includes appropriate error handling
- Has comprehensive test coverage
- Is well documented

## Common Use Cases

- **Network Protocol Parsing**: Parse protocol messages with TLV encoding
- **Configuration Files**: Read binary configuration data
- **Data Serialization**: Deserialize application data
- **File Format Parsing**: Parse file formats using TLV encoding
- **Message Queuing**: Parse message payloads in messaging systems