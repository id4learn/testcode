#include "tlv_parser.hpp"
#include <iostream>

int main() {
    using namespace tlv;
    
    std::cout << "Simple TLV Parser Test\n";
    std::cout << "=====================\n\n";
    
    try {
        // Build some TLV data
        TlvBuilder builder;
        builder.add(0x01, std::string("Hello"))
               .add(0x02, static_cast<uint32_t>(42))
               .add(0x03, std::string("World"));
        
        std::cout << "Created TLV data with " << builder.size() << " bytes\n";
        
        // Parse the data
        TlvParser parser;
        auto elements = parser.parse_all(builder.data());
        
        std::cout << "Parsed " << elements.size() << " elements:\n";
        
        for (const auto& element : elements) {
            std::cout << "  Tag 0x" << std::hex << element.tag() << std::dec 
                      << ": ";
            
            if (element.tag() == 0x02) {
                std::cout << "Integer value: " << element.value_as_integer<uint32_t>();
            } else {
                std::cout << "String value: '" << element.value_as_string() << "'";
            }
            std::cout << " (" << element.length() << " bytes)\n";
        }
        
        // Test finding specific tags
        auto hello_element = parser.find_by_tag(builder.data(), 0x01);
        if (hello_element) {
            std::cout << "\nFound tag 0x01: '" << hello_element->value_as_string() << "'\n";
        }
        
        // Validate the data
        bool valid = parser.validate(builder.data());
        std::cout << "Data validation: " << (valid ? "PASSED" : "FAILED") << "\n";
        
        std::cout << "\nTest completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}