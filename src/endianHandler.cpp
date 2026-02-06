#include "endianHandler.hpp"

uint8_t LittleEndian_TIFF::convert(std::array<char, 1> c) const {
    return uint8_t(c.at(0));
}

ushort_t LittleEndian_TIFF::convert(std::array<char, 2> c) const {
    return (ushort_t(uint8_t(c.at(1))) << 8) + ushort_t(uint8_t(c.at(0)));
}

uint_t LittleEndian_TIFF::convert(std::array<char, 4> c) const {
    return  (uint_t(uint8_t(c.at(3))) << 24) + 
            (uint_t(uint8_t(c.at(2))) << 16) + 
            (uint_t(uint8_t(c.at(1))) << 8) + 
            uint_t(uint8_t(c.at(0)));
}

uint64_t LittleEndian_TIFF::convert(std::array<char, 8> c) const {
    return  (uint64_t(uint8_t(c.at(7))) << 56) + 
            (uint64_t(uint8_t(c.at(6))) << 48) + 
            (uint64_t(uint8_t(c.at(5))) << 40) + 
            (uint64_t(uint8_t(c.at(4))) << 32) + 
            (uint64_t(uint8_t(c.at(3))) << 24) + 
            (uint64_t(uint8_t(c.at(2))) << 16) + 
            (uint64_t(uint8_t(c.at(1))) << 8) + 
            uint64_t(uint8_t(c.at(0)));
}

ushort_t LittleEndian_TIFF::convert(uint_t c, size_t idx) const {
    if(idx == 1){
        return ushort_t(c >> 8);
    } else if (idx == 0) {
        return ushort_t(c);
    }
    throw std::runtime_error("Error only 0 <= idx <= 1 are allowed");
}

std::array<char, 1> LittleEndian_TIFF::convert_to_array(uint8_t v) const {
    return std::array<char,1>{char(v)};
}

std::array<char, 2> LittleEndian_TIFF::convert_to_array(ushort_t v) const {
    std::array<char,2> a;
    a.at(0) = char( v & 0xFF);
    a.at(1) = char( (v >> 8) & 0xFF);

    return a;
}

std::array<char, 4> LittleEndian_TIFF::convert_to_array(uint_t v) const {
    std::array<char,4> a;
    a.at(0) = char( v & 0xFF);
    a.at(1) = char( (v >> 8) & 0xFF);
    a.at(2) = char( (v >> 16) & 0xFF);
    a.at(3) = char( (v >> 24) & 0xFF);

    return a;
}

std::array<char, 8> LittleEndian_TIFF::convert_to_array(uint64_t v) const {
    std::array<char,8> a;
    a.at(0) = char( v & 0xFF);
    a.at(1) = char( (v >> 8) & 0xFF);
    a.at(2) = char( (v >> 16) & 0xFF);
    a.at(3) = char( (v >> 24) & 0xFF);
    a.at(4) = char( (v >> 32) & 0xFF);
    a.at(5) = char( (v >> 40) & 0xFF);
    a.at(6) = char( (v >> 48) & 0xFF);
    a.at(7) = char( (v >> 54) & 0xFF);

    return a;
}