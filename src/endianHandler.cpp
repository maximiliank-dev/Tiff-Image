#include "endianHandler.hpp"

uint8_t LittleEndian_TIFF::convert(std::array<char, 1> c) const {
    return uint8_t(c.at(0));
}

ushort_t LittleEndian_TIFF::convert(std::array<char, 2> c) const {
    return (ushort_t(uint8_t(c.at(1))) << 8) + ushort_t(uint8_t(c.at(0)));
}

uint_t LittleEndian_TIFF::convert(std::array<char, 4> c) const {
    return (uint_t(uint8_t(c.at(3))) << 24) + (uint_t(uint8_t(c.at(2))) << 16) +
           (uint_t(uint8_t(c.at(1))) << 8) + uint_t(uint8_t(c.at(0)));
}

uint64_t LittleEndian_TIFF::convert(std::array<char, 8> c) const {
    return (uint64_t(uint8_t(c.at(7))) << 56) +
           (uint64_t(uint8_t(c.at(6))) << 48) +
           (uint64_t(uint8_t(c.at(5))) << 40) +
           (uint64_t(uint8_t(c.at(4))) << 32) +
           (uint64_t(uint8_t(c.at(3))) << 24) +
           (uint64_t(uint8_t(c.at(2))) << 16) +
           (uint64_t(uint8_t(c.at(1))) << 8) + uint64_t(uint8_t(c.at(0)));
}

//handles that in the TIFF spec in a 4 byte field
uint8_t LittleEndian_TIFF::convert8(uint_t c, size_t idx) const {
    if (idx == 3) {
        return uint8_t(c >> 24);
    } else if (idx == 2) {
        return uint8_t(c >> 16);
    } else if (idx == 1) {
        return uint8_t(c >> 8);
    } else if (idx == 0) {
        return uint8_t(c);
    }
    throw std::runtime_error("Error only 0 <= idx <= 3 are allowed");
}

//handles that in the TIFF spec in a 4 byte field, the shorts are stored starting in the least significant bit
// bytes 0-1 short 0, bytes 2-3 short 1 
ushort_t LittleEndian_TIFF::convert(uint_t c, size_t idx) const {
    // c = B0 | (B1<<8) | (B2<<16) | (B3<<24)
    if (idx == 1) {
        return ushort_t(c >> 16);
    } else if (idx == 0) {
        return ushort_t(c);
    }
    throw std::runtime_error("Error only 0 <= idx <= 1 are allowed");
}

std::array<char, 1> LittleEndian_TIFF::convert_to_array(uint8_t v) const {
    return std::array<char, 1>{char(v)};
}

std::array<char, 2> LittleEndian_TIFF::convert_to_array(ushort_t v) const {
    std::array<char, 2> a;
    a.at(0) = char(v & 0xFF);
    a.at(1) = char((v >> 8) & 0xFF);

    return a;
}

std::array<char, 4> LittleEndian_TIFF::convert_to_array(uint_t v) const {
    std::array<char, 4> a;
    a.at(0) = char(v & 0xFF);
    a.at(1) = char((v >> 8) & 0xFF);
    a.at(2) = char((v >> 16) & 0xFF);
    a.at(3) = char((v >> 24) & 0xFF);

    return a;
}

std::array<char, 8> LittleEndian_TIFF::convert_to_array(uint64_t v) const {
    std::array<char, 8> a;
    a.at(0) = char(v & 0xFF);
    a.at(1) = char((v >> 8) & 0xFF);
    a.at(2) = char((v >> 16) & 0xFF);
    a.at(3) = char((v >> 24) & 0xFF);
    a.at(4) = char((v >> 32) & 0xFF);
    a.at(5) = char((v >> 40) & 0xFF);
    a.at(6) = char((v >> 48) & 0xFF);
    a.at(7) = char((v >> 54) & 0xFF);

    return a;
}

tiff_header_endian LittleEndian_TIFF::get_endian_value() const {
    return tiff_header_endian::LITTLE;
}

uint8_t BigEndian_TIFF::convert(std::array<char, 1> c) const {
    return uint8_t(c.at(0));
}

ushort_t BigEndian_TIFF::convert(std::array<char, 2> c) const {
    return (ushort_t(uint8_t(c.at(0))) << 8) + ushort_t(uint8_t(c.at(1)));
}

uint_t BigEndian_TIFF::convert(std::array<char, 4> c) const {
    return (uint_t(uint8_t(c.at(0))) << 24) + (uint_t(uint8_t(c.at(1))) << 16) +
           (uint_t(uint8_t(c.at(2))) << 8) + uint_t(uint8_t(c.at(3)));
}

uint64_t BigEndian_TIFF::convert(std::array<char, 8> c) const {
    return (uint64_t(uint8_t(c.at(0))) << 56) +
           (uint64_t(uint8_t(c.at(1))) << 48) +
           (uint64_t(uint8_t(c.at(2))) << 40) +
           (uint64_t(uint8_t(c.at(3))) << 32) +
           (uint64_t(uint8_t(c.at(4))) << 24) +
           (uint64_t(uint8_t(c.at(5))) << 16) +
           (uint64_t(uint8_t(c.at(6))) << 8) + uint64_t(uint8_t(c.at(7)));
}

//handles the 4 byte filed like in the tiff spec defined
uint8_t BigEndian_TIFF::convert8(uint_t c, size_t idx) const {
    // c = (B0<<24) | (B1<<16) | (B2<<8) | B3
    if (idx == 0) {
        return uint8_t(c >> 24);
    } else if (idx == 1) {
        return uint8_t(c >> 16);
    } else if (idx == 2) {
        return uint8_t(c >> 8);
    } else if (idx == 3) {
        return uint8_t(c);
    }
    throw std::runtime_error("Error only 0 <= idx <= 3 are allowed");
}

//handles the 4 byte filed like in the tiff spec defined
ushort_t BigEndian_TIFF::convert(uint_t c, size_t idx) const {
    // c = (B0<<24) | (B1<<16) | (B2<<8) | B3
    if (idx == 0) {
        return ushort_t(c >> 16);
    } else if (idx == 1) {
        return ushort_t(c);
    }
    throw std::runtime_error("Error only 0 <= idx <= 1 are allowed");
}

std::array<char, 1> BigEndian_TIFF::convert_to_array(uint8_t v) const {
    return std::array<char, 1>{char(v)};
}

std::array<char, 2> BigEndian_TIFF::convert_to_array(ushort_t v) const {
    std::array<char, 2> a;
    a.at(0) = char((v >> 8) & 0xFF);
    a.at(1) = char(v & 0xFF);
    return a;
}

std::array<char, 4> BigEndian_TIFF::convert_to_array(uint_t v) const {
    std::array<char, 4> a;
    a.at(0) = char((v >> 24) & 0xFF);
    a.at(1) = char((v >> 16) & 0xFF);
    a.at(2) = char((v >> 8) & 0xFF);
    a.at(3) = char(v & 0xFF);
    return a;
}

std::array<char, 8> BigEndian_TIFF::convert_to_array(uint64_t v) const {
    std::array<char, 8> a;
    a.at(0) = char((v >> 56) & 0xFF);
    a.at(1) = char((v >> 48) & 0xFF);
    a.at(2) = char((v >> 40) & 0xFF);
    a.at(3) = char((v >> 32) & 0xFF);
    a.at(4) = char((v >> 24) & 0xFF);
    a.at(5) = char((v >> 16) & 0xFF);
    a.at(6) = char((v >> 8) & 0xFF);
    a.at(7) = char(v & 0xFF);
    return a;
}

tiff_header_endian BigEndian_TIFF::get_endian_value() const {
    return tiff_header_endian::BIG;
}