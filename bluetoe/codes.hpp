#ifndef BLUETOE_CODES_HPP
#define BLUETOE_CODES_HPP

#include <cstdint>

namespace bluetoe
{
    enum class att_opcodes : std::uint8_t {
        error_response = 1
    };

    inline std::uint8_t bits( att_opcodes c )
    {
        return static_cast< std::uint8_t >( c );
    }

    enum class att_error_codes : std::uint8_t {
        invalid_handle                      = 0x01,
        read_not_permitted,
        write_not_permitted,
        invalid_pdu,
        insufficient_authentication,
        request_not_supported,
        invalid_offset,
        insufficient_authorization,
        prepare_queue_full,
        attribute_not_found,
        attribute_not_long,
        insufficient_encryption_key_size,
        invalid_attribute_value_length,
        unlikely_error,
        insufficient_encryption,
        unsupported_group_type,
        insufficient_resources
    };

    inline std::uint8_t bits( att_error_codes c )
    {
        return static_cast< std::uint8_t >( c );
    }
}

#endif