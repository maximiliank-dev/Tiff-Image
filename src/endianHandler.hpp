
#pragma once

#include <array>
#include <stdexcept>

#include "tiff-config/types.hpp"
#include "tiff-config/tiff-types.hpp"

class VirtualEndianHandler {
/**
 * mark all member functions constant, since no member variable is used
 */
public:
    //conversion from an array
    virtual ushort_t convert(uint_t c, size_t idx) const = 0;
    virtual uint8_t convert(std::array<char, 1> c) const = 0;
    virtual ushort_t convert(std::array<char, 2> c) const = 0;
    virtual uint_t convert(std::array<char, 4> c) const = 0;
    virtual uint64_t convert(std::array<char, 8> c) const = 0;

    // conversions to an array
    virtual std::array<char, 1> convert_to_array(uint8_t v) const = 0;
    virtual std::array<char, 2> convert_to_array(ushort_t v) const = 0;
    virtual std::array<char, 4> convert_to_array(uint_t v) const = 0;
    virtual std::array<char, 8> convert_to_array(uint64_t v) const = 0;

    virtual tiff_header_endian get_endian_value() const = 0;

    //ASCII characters are not effected by the endianess , so there is no implementation needed

    virtual ~VirtualEndianHandler() = default;
};


class LittleEndian_TIFF : public VirtualEndianHandler {
    uint8_t convert(std::array<char, 1> c) const override;
    ushort_t convert(std::array<char, 2> c) const override;
    uint_t convert(std::array<char, 4> c) const override;
    uint64_t convert(std::array<char, 8> c) const override;
    ushort_t convert(uint_t c, size_t idx) const override;


    std::array<char, 1> convert_to_array(uint8_t v) const override;
    std::array<char, 2> convert_to_array(ushort_t v) const override;
    std::array<char, 4> convert_to_array(uint_t v) const override;
    std::array<char, 8> convert_to_array(uint64_t v) const override;

    tiff_header_endian get_endian_value() const override;
};

