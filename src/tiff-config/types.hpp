#pragma once

#include <cstdint>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

using uint8_t = std::uint8_t;
using uint_t = std::uint32_t;
using ushort_t = std::uint16_t;
using uint64_t = std::uint64_t;

template <unsigned int N>
bool read_char(std::array<char, N>& buffer, std::basic_istream<char>& stream) {
    stream.read(buffer.data(), buffer.size());

    if (!stream) {
        throw std::runtime_error("error stream is invalid");
    }
    return 1;
}

template <unsigned int N>
inline bool write_char(std::array<char, N> data,
                       std::basic_ostream<char>& stream) {
    stream.write(data.data(), data.size());

    if (!stream) {
        throw std::runtime_error("error stream is invalid");
    }
    return 1;
}

inline bool write_char(std::vector<char>& data,
                       std::basic_ostream<char>& stream) {
    stream.write(data.data(), data.size());

    if (!stream) {
        throw std::runtime_error("error stream is invalid");
    }
    return 1;
}

inline bool write_char(std::string& data, std::basic_ostream<char>& stream) {
    stream.write(data.data(), data.size());

    if (!stream) {
        throw std::runtime_error("error stream is invalid");
    }
    return 1;
}

inline bool write_char(char data, std::basic_ostream<char>& stream) {
    char d = data;
    stream.write(&d, 1);

    if (!stream) {
        throw std::runtime_error("error stream is invalid");
    }
    return 1;
}
