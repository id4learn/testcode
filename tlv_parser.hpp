#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <span>
#include <optional>
#include <string>
#include <memory>

namespace tlv {

/**
 * @brief Exception thrown when TLV parsing encounters an error
 */
class TlvParseError : public std::runtime_error {
public:
    explicit TlvParseError(const std::string& message) 
        : std::runtime_error("TLV Parse Error: " + message) {}
};

/**
 * @brief Represents the endianness for multi-byte values
 */
enum class Endianness {
    BigEndian,
    LittleEndian,
    Native
};

/**
 * @brief Configuration for TLV parsing
 */
struct TlvConfig {
    uint8_t tag_size = 1;           // Size of tag field in bytes (1, 2, or 4)
    uint8_t length_size = 1;        // Size of length field in bytes (1, 2, or 4)
    Endianness endianness = Endianness::BigEndian;
    bool strict_validation = true;   // Whether to perform strict validation
};

/**
 * @brief Represents a single TLV element
 */
class TlvElement {
private:
    uint32_t tag_;
    uint32_t length_;
    std::span<const uint8_t> value_;

public:
    TlvElement(uint32_t tag, uint32_t length, std::span<const uint8_t> value)
        : tag_(tag), length_(length), value_(value) {}

    /**
     * @brief Get the tag value
     */
    uint32_t tag() const noexcept { return tag_; }

    /**
     * @brief Get the length of the value
     */
    uint32_t length() const noexcept { return length_; }

    /**
     * @brief Get the raw value data
     */
    std::span<const uint8_t> value() const noexcept { return value_; }

    /**
     * @brief Get the value as a string (assuming UTF-8 encoding)
     */
    std::string value_as_string() const {
        return std::string(reinterpret_cast<const char*>(value_.data()), value_.size());
    }

    /**
     * @brief Get the value as an integer (assuming big-endian encoding)
     * @tparam T Integer type to convert to
     * @throws TlvParseError if value size doesn't match type size
     */
    template<typename T>
    T value_as_integer() const {
        static_assert(std::is_integral_v<T>, "T must be an integral type");
        
        if (value_.size() != sizeof(T)) {
            throw TlvParseError("Value size (" + std::to_string(value_.size()) + 
                              ") doesn't match expected size (" + std::to_string(sizeof(T)) + ")");
        }
        
        T result = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            result = (result << 8) | value_[i];
        }
        return result;
    }

    /**
     * @brief Check if this TLV element contains nested TLV data
     */
    bool has_nested_tlv() const noexcept {
        return length_ > 0 && length_ >= 2; // Minimum size for nested TLV (1 byte tag + 1 byte length)
    }
};

/**
 * @brief Iterator for TLV elements in a data buffer
 */
class TlvIterator {
private:
    std::span<const uint8_t> data_;
    size_t position_;
    TlvConfig config_;
    std::optional<TlvElement> current_element_;

    void advance();
    uint32_t read_field(size_t field_size);

public:
    TlvIterator(std::span<const uint8_t> data, const TlvConfig& config);
    
    /**
     * @brief Check if iterator has more elements
     */
    bool has_next() const noexcept { return current_element_.has_value(); }

    /**
     * @brief Get the current TLV element
     * @throws TlvParseError if no current element
     */
    const TlvElement& current() const {
        if (!current_element_) {
            throw TlvParseError("No current element available");
        }
        return *current_element_;
    }

    /**
     * @brief Move to the next TLV element
     * @return Reference to this iterator
     */
    TlvIterator& operator++() {
        advance();
        return *this;
    }

    /**
     * @brief Get current position in the data buffer
     */
    size_t position() const noexcept { return position_; }
};

/**
 * @brief Main TLV parser class
 */
class TlvParser {
private:
    TlvConfig config_;

public:
    /**
     * @brief Construct TLV parser with default configuration
     */
    TlvParser() = default;

    /**
     * @brief Construct TLV parser with custom configuration
     */
    explicit TlvParser(const TlvConfig& config) : config_(config) {}

    /**
     * @brief Parse TLV data and return all elements
     * @param data Input data buffer
     * @return Vector of parsed TLV elements
     * @throws TlvParseError on parsing errors
     */
    std::vector<TlvElement> parse_all(std::span<const uint8_t> data) const;

    /**
     * @brief Create an iterator for the TLV data
     * @param data Input data buffer
     * @return TLV iterator
     */
    TlvIterator create_iterator(std::span<const uint8_t> data) const {
        return TlvIterator(data, config_);
    }

    /**
     * @brief Find the first TLV element with the specified tag
     * @param data Input data buffer
     * @param tag Tag to search for
     * @return Optional TLV element (empty if not found)
     */
    std::optional<TlvElement> find_by_tag(std::span<const uint8_t> data, uint32_t tag) const;

    /**
     * @brief Find all TLV elements with the specified tag
     * @param data Input data buffer
     * @param tag Tag to search for
     * @return Vector of matching TLV elements
     */
    std::vector<TlvElement> find_all_by_tag(std::span<const uint8_t> data, uint32_t tag) const;

    /**
     * @brief Validate TLV data structure
     * @param data Input data buffer
     * @return True if data is valid TLV format
     */
    bool validate(std::span<const uint8_t> data) const noexcept;

    /**
     * @brief Get the current configuration
     */
    const TlvConfig& config() const noexcept { return config_; }

    /**
     * @brief Update parser configuration
     */
    void set_config(const TlvConfig& config) { config_ = config; }
};

/**
 * @brief Utility function to create TLV data
 */
class TlvBuilder {
private:
    std::vector<uint8_t> buffer_;
    TlvConfig config_;

    void write_field(uint32_t value, size_t field_size);

public:
    explicit TlvBuilder(const TlvConfig& config = {}) : config_(config) {}

    /**
     * @brief Add a TLV element with binary data
     */
    TlvBuilder& add(uint32_t tag, std::span<const uint8_t> value);

    /**
     * @brief Add a TLV element with string data
     */
    TlvBuilder& add(uint32_t tag, const std::string& value);

    /**
     * @brief Add a TLV element with integer data
     */
    template<typename T>
    TlvBuilder& add(uint32_t tag, T value) {
        static_assert(std::is_integral_v<T>, "T must be an integral type");
        
        std::vector<uint8_t> bytes(sizeof(T));
        for (int i = sizeof(T) - 1; i >= 0; --i) {
            bytes[sizeof(T) - 1 - i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        }
        
        return add(tag, std::span<const uint8_t>(bytes));
    }

    /**
     * @brief Get the built TLV data
     */
    const std::vector<uint8_t>& data() const noexcept { return buffer_; }

    /**
     * @brief Clear the builder
     */
    void clear() { buffer_.clear(); }

    /**
     * @brief Get the size of the built data
     */
    size_t size() const noexcept { return buffer_.size(); }
};

} // namespace tlv