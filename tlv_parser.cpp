#include "tlv_parser.hpp"
#include <algorithm>
#include <cstring>

namespace tlv {

// Helper function to convert endianness
uint32_t convert_endianness(uint32_t value, size_t size, Endianness endianness) {
    if (endianness == Endianness::Native || endianness == Endianness::BigEndian) {
        return value;
    }
    
    // Little endian conversion
    uint32_t result = 0;
    for (size_t i = 0; i < size; ++i) {
        result |= ((value >> (i * 8)) & 0xFF) << ((size - 1 - i) * 8);
    }
    return result;
}

// TlvIterator implementation
TlvIterator::TlvIterator(std::span<const uint8_t> data, const TlvConfig& config)
    : data_(data), position_(0), config_(config) {
    
    // Validate configuration
    if (config_.tag_size != 1 && config_.tag_size != 2 && config_.tag_size != 4) {
        throw TlvParseError("Invalid tag size: " + std::to_string(config_.tag_size));
    }
    if (config_.length_size != 1 && config_.length_size != 2 && config_.length_size != 4) {
        throw TlvParseError("Invalid length size: " + std::to_string(config_.length_size));
    }
    
    // Try to read the first element
    advance();
}

uint32_t TlvIterator::read_field(size_t field_size) {
    if (position_ + field_size > data_.size()) {
        throw TlvParseError("Unexpected end of data while reading field");
    }
    
    uint32_t value = 0;
    for (size_t i = 0; i < field_size; ++i) {
        value = (value << 8) | data_[position_ + i];
    }
    
    value = convert_endianness(value, field_size, config_.endianness);
    position_ += field_size;
    return value;
}

void TlvIterator::advance() {
    current_element_.reset();
    
    // Check if we have enough data for tag and length
    if (position_ + config_.tag_size + config_.length_size > data_.size()) {
        return; // End of data
    }
    
    try {
        // Read tag
        uint32_t tag = read_field(config_.tag_size);
        
        // Read length
        uint32_t length = read_field(config_.length_size);
        
        // Validate length
        if (position_ + length > data_.size()) {
            if (config_.strict_validation) {
                throw TlvParseError("Value length (" + std::to_string(length) + 
                                  ") exceeds remaining data size (" + 
                                  std::to_string(data_.size() - position_) + ")");
            } else {
                return; // Silently ignore incomplete data in non-strict mode
            }
        }
        
        // Create span for value data
        std::span<const uint8_t> value_span(data_.data() + position_, length);
        
        // Create TLV element
        current_element_.emplace(tag, length, value_span);
        
        // Move position past the value
        position_ += length;
        
    } catch (const TlvParseError&) {
        current_element_.reset();
        throw;
    }
}

// TlvParser implementation
std::vector<TlvElement> TlvParser::parse_all(std::span<const uint8_t> data) const {
    std::vector<TlvElement> elements;
    
    try {
        TlvIterator iterator = create_iterator(data);
        while (iterator.has_next()) {
            elements.push_back(iterator.current());
            ++iterator;
        }
    } catch (const TlvParseError&) {
        throw;
    }
    
    return elements;
}

std::optional<TlvElement> TlvParser::find_by_tag(std::span<const uint8_t> data, uint32_t tag) const {
    try {
        TlvIterator iterator = create_iterator(data);
        while (iterator.has_next()) {
            const auto& element = iterator.current();
            if (element.tag() == tag) {
                return element;
            }
            ++iterator;
        }
    } catch (const TlvParseError&) {
        throw;
    }
    
    return std::nullopt;
}

std::vector<TlvElement> TlvParser::find_all_by_tag(std::span<const uint8_t> data, uint32_t tag) const {
    std::vector<TlvElement> matching_elements;
    
    try {
        TlvIterator iterator = create_iterator(data);
        while (iterator.has_next()) {
            const auto& element = iterator.current();
            if (element.tag() == tag) {
                matching_elements.push_back(element);
            }
            ++iterator;
        }
    } catch (const TlvParseError&) {
        throw;
    }
    
    return matching_elements;
}

bool TlvParser::validate(std::span<const uint8_t> data) const noexcept {
    try {
        // Create a copy of config with strict validation enabled
        TlvConfig strict_config = config_;
        strict_config.strict_validation = true;
        
        TlvIterator iterator(data, strict_config);
        while (iterator.has_next()) {
            ++iterator;
        }
        return true;
    } catch (...) {
        return false;
    }
}

// TlvBuilder implementation
void TlvBuilder::write_field(uint32_t value, size_t field_size) {
    value = convert_endianness(value, field_size, config_.endianness);
    
    for (int i = field_size - 1; i >= 0; --i) {
        buffer_.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
    }
}

TlvBuilder& TlvBuilder::add(uint32_t tag, std::span<const uint8_t> value) {
    // Validate tag size
    if (config_.tag_size == 1 && tag > 0xFF) {
        throw TlvParseError("Tag value too large for configured tag size");
    } else if (config_.tag_size == 2 && tag > 0xFFFF) {
        throw TlvParseError("Tag value too large for configured tag size");
    }
    
    // Validate length size
    uint32_t length = static_cast<uint32_t>(value.size());
    if (config_.length_size == 1 && length > 0xFF) {
        throw TlvParseError("Value length too large for configured length size");
    } else if (config_.length_size == 2 && length > 0xFFFF) {
        throw TlvParseError("Value length too large for configured length size");
    }
    
    // Write tag
    write_field(tag, config_.tag_size);
    
    // Write length
    write_field(length, config_.length_size);
    
    // Write value
    buffer_.insert(buffer_.end(), value.begin(), value.end());
    
    return *this;
}

TlvBuilder& TlvBuilder::add(uint32_t tag, const std::string& value) {
    const uint8_t* data = reinterpret_cast<const uint8_t*>(value.c_str());
    return add(tag, std::span<const uint8_t>(data, value.length()));
}

} // namespace tlv