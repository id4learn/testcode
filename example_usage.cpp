#include "tlv_parser.hpp"
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace tlv;

// Helper function to print binary data in hex format
void print_hex(const std::vector<uint8_t>& data) {
    for (const auto& byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void print_hex(std::span<const uint8_t> data) {
    for (const auto& byte : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
    }
    std::cout << std::dec << std::endl;
}

void example_basic_usage() {
    std::cout << "\n=== Basic TLV Usage Example ===\n";
    
    // Create TLV data using the builder
    TlvBuilder builder;
    builder.add(0x01, std::string("Hello World"))           // Tag 1: String value
           .add(0x02, static_cast<uint16_t>(1234))    // Tag 2: 16-bit integer
           .add(0x03, static_cast<uint32_t>(0x12345678)); // Tag 3: 32-bit integer
    
    const auto& tlv_data = builder.data();
    std::cout << "Built TLV data: ";
    print_hex(tlv_data);
    
    // Parse the TLV data
    TlvParser parser;
    
    try {
        auto elements = parser.parse_all(tlv_data);
        
        std::cout << "Parsed " << elements.size() << " TLV elements:\n";
        for (const auto& element : elements) {
            std::cout << "  Tag: 0x" << std::hex << element.tag() 
                      << ", Length: " << std::dec << element.length() << "\n";
            
            switch (element.tag()) {
                case 0x01:
                    std::cout << "    String value: '" << element.value_as_string() << "'\n";
                    break;
                case 0x02:
                    std::cout << "    16-bit integer: " << element.value_as_integer<uint16_t>() << "\n";
                    break;
                case 0x03:
                    std::cout << "    32-bit integer: 0x" << std::hex 
                              << element.value_as_integer<uint32_t>() << std::dec << "\n";
                    break;
            }
        }
    } catch (const TlvParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

void example_iterator_usage() {
    std::cout << "\n=== Iterator Usage Example ===\n";
    
    // Create TLV data with multiple elements of the same tag
    TlvBuilder builder;
    builder.add(0x10, std::string("First"))
           .add(0x10, std::string("Second"))
           .add(0x20, std::string("Different tag"))
           .add(0x10, std::string("Third"));
    
    const auto& tlv_data = builder.data();
    std::cout << "TLV data with repeated tags: ";
    print_hex(tlv_data);
    
    TlvParser parser;
    
    try {
        // Use iterator to traverse all elements
        std::cout << "Using iterator:\n";
        auto iterator = parser.create_iterator(tlv_data);
        int count = 0;
        while (iterator.has_next()) {
            const auto& element = iterator.current();
            std::cout << "  Element " << ++count 
                      << " - Tag: 0x" << std::hex << element.tag() 
                      << ", Value: '" << element.value_as_string() << "'" << std::dec << "\n";
            ++iterator;
        }
        
        // Find specific elements by tag
        std::cout << "\nFinding elements with tag 0x10:\n";
        auto tag_10_elements = parser.find_all_by_tag(tlv_data, 0x10);
        for (size_t i = 0; i < tag_10_elements.size(); ++i) {
            std::cout << "  Found #" << (i + 1) 
                      << ": '" << tag_10_elements[i].value_as_string() << "'\n";
        }
        
        // Find first element with specific tag
        auto first_tag_20 = parser.find_by_tag(tlv_data, 0x20);
        if (first_tag_20) {
            std::cout << "\nFirst element with tag 0x20: '" 
                      << first_tag_20->value_as_string() << "'\n";
        }
        
    } catch (const TlvParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

void example_different_configurations() {
    std::cout << "\n=== Different Configuration Example ===\n";
    
    // Use 2-byte tags and 2-byte lengths
    TlvConfig config;
    config.tag_size = 2;
    config.length_size = 2;
    config.endianness = Endianness::BigEndian;
    
    TlvBuilder builder(config);
    builder.add(0x1234, std::string("Large tag value"))
           .add(0x5678, static_cast<uint64_t>(0x123456789ABCDEF0));
    
    const auto& tlv_data = builder.data();
    std::cout << "TLV data with 2-byte tags and lengths: ";
    print_hex(tlv_data);
    
    TlvParser parser(config);
    
    try {
        auto elements = parser.parse_all(tlv_data);
        
        for (const auto& element : elements) {
            std::cout << "  Tag: 0x" << std::hex << element.tag() 
                      << ", Length: " << std::dec << element.length() << "\n";
            
            if (element.tag() == 0x1234) {
                std::cout << "    String: '" << element.value_as_string() << "'\n";
            } else if (element.tag() == 0x5678) {
                std::cout << "    64-bit value: 0x" << std::hex 
                          << element.value_as_integer<uint64_t>() << std::dec << "\n";
            }
        }
    } catch (const TlvParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

void example_nested_tlv() {
    std::cout << "\n=== Nested TLV Example ===\n";
    
    // Create inner TLV structure
    TlvBuilder inner_builder;
    inner_builder.add(0x01, std::string("Inner value 1"))
                 .add(0x02, std::string("Inner value 2"));
    
    // Create outer TLV structure containing the inner one
    TlvBuilder outer_builder;
    const auto& inner_data = inner_builder.data();
    outer_builder.add(0x10, std::string("Outer value"))
                 .add(0x20, std::span<const uint8_t>(inner_data))  // Nested TLV data
                 .add(0x30, std::string("Another outer value"));
    
    const auto& tlv_data = outer_builder.data();
    std::cout << "Nested TLV data: ";
    print_hex(tlv_data);
    
    TlvParser parser;
    
    try {
        auto elements = parser.parse_all(tlv_data);
        
        for (const auto& element : elements) {
            std::cout << "Tag: 0x" << std::hex << element.tag() << std::dec 
                      << ", Length: " << element.length() << "\n";
            
            if (element.tag() == 0x20 && element.has_nested_tlv()) {
                std::cout << "  Contains nested TLV data:\n";
                
                // Parse nested TLV
                auto nested_elements = parser.parse_all(element.value());
                for (const auto& nested : nested_elements) {
                    std::cout << "    Nested Tag: 0x" << std::hex << nested.tag() 
                              << ", Value: '" << nested.value_as_string() << "'" << std::dec << "\n";
                }
            } else {
                std::cout << "  Value: '" << element.value_as_string() << "'\n";
            }
        }
    } catch (const TlvParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

void example_error_handling() {
    std::cout << "\n=== Error Handling Example ===\n";
    
    // Create intentionally malformed TLV data
    std::vector<uint8_t> malformed_data = {
        0x01,        // Tag
        0x10,        // Length (16 bytes)
        0x41, 0x42   // Only 2 bytes of data (should be 16)
    };
    
    std::cout << "Malformed TLV data: ";
    print_hex(malformed_data);
    
    TlvParser parser;
    
    // Test validation
    bool is_valid = parser.validate(malformed_data);
    std::cout << "Data validation result: " << (is_valid ? "VALID" : "INVALID") << "\n";
    
    // Test strict parsing (should throw)
    try {
        auto elements = parser.parse_all(malformed_data);
        std::cout << "Unexpectedly parsed successfully!\n";
    } catch (const TlvParseError& e) {
        std::cout << "Expected parse error: " << e.what() << "\n";
    }
    
    // Test non-strict parsing
    TlvConfig non_strict_config;
    non_strict_config.strict_validation = false;
    TlvParser non_strict_parser(non_strict_config);
    
    try {
        auto elements = non_strict_parser.parse_all(malformed_data);
        std::cout << "Non-strict parser parsed " << elements.size() << " elements\n";
    } catch (const TlvParseError& e) {
        std::cout << "Non-strict parse error: " << e.what() << "\n";
    }
}

void example_real_world_scenario() {
    std::cout << "\n=== Real World Scenario: Network Protocol Message ===\n";
    
    // Simulate a network protocol message with various field types
    TlvBuilder message_builder;
    
    // Protocol version (1 byte)
    message_builder.add(0x01, static_cast<uint8_t>(2));
    
    // Message ID (4 bytes)
    message_builder.add(0x02, static_cast<uint32_t>(0x12345678));
    
    // Timestamp (8 bytes)
    message_builder.add(0x03, static_cast<uint64_t>(1640995200000ULL)); // 2022-01-01 00:00:00 UTC
    
    // Source address (IPv4)
    std::vector<uint8_t> ipv4_addr = {192, 168, 1, 100};
    message_builder.add(0x04, std::span<const uint8_t>(ipv4_addr));
    
    // User name
    message_builder.add(0x05, std::string("john.doe@example.com"));
    
    // Optional data payload
    std::string payload = "This is some application data payload";
    message_builder.add(0x06, payload);
    
    const auto& message_data = message_builder.data();
    std::cout << "Protocol message (" << message_data.size() << " bytes): ";
    print_hex(message_data);
    
    // Parse the message
    TlvParser parser;
    
    try {
        std::cout << "\nParsed message fields:\n";
        
        auto version = parser.find_by_tag(message_data, 0x01);
        if (version) {
            std::cout << "  Protocol Version: " << static_cast<int>(version->value_as_integer<uint8_t>()) << "\n";
        }
        
        auto msg_id = parser.find_by_tag(message_data, 0x02);
        if (msg_id) {
            std::cout << "  Message ID: 0x" << std::hex << msg_id->value_as_integer<uint32_t>() << std::dec << "\n";
        }
        
        auto timestamp = parser.find_by_tag(message_data, 0x03);
        if (timestamp) {
            std::cout << "  Timestamp: " << timestamp->value_as_integer<uint64_t>() << "\n";
        }
        
        auto src_addr = parser.find_by_tag(message_data, 0x04);
        if (src_addr && src_addr->length() == 4) {
            auto addr_data = src_addr->value();
            std::cout << "  Source IP: " << static_cast<int>(addr_data[0]) << "."
                      << static_cast<int>(addr_data[1]) << "."
                      << static_cast<int>(addr_data[2]) << "."
                      << static_cast<int>(addr_data[3]) << "\n";
        }
        
        auto username = parser.find_by_tag(message_data, 0x05);
        if (username) {
            std::cout << "  Username: " << username->value_as_string() << "\n";
        }
        
        auto data_payload = parser.find_by_tag(message_data, 0x06);
        if (data_payload) {
            std::cout << "  Payload: " << data_payload->value_as_string() << "\n";
        }
        
    } catch (const TlvParseError& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "TLV Parser Examples\n";
    std::cout << "==================\n";
    
    try {
        example_basic_usage();
        example_iterator_usage();
        example_different_configurations();
        example_nested_tlv();
        example_error_handling();
        example_real_world_scenario();
        
        std::cout << "\n=== All examples completed successfully! ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}