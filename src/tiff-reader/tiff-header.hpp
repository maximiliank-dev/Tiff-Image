
#pragma once

#include <iostream>
#include <bit>
#include <array>
#include <memory>
#include <format>

#include "tiff-config/types.hpp"
#include "endianHandler.hpp"

class TIFFHeader {

private:
    std::basic_istream<char>& _stream;

    //header
    std::array<char, 2> endian = {0 ,0};
    ushort_t magic_num = 0;
    ushort_t version = 0;
    uint_t idf_offset = 0;

    bool err = 0;

    std::shared_ptr<VirtualEndianHandler> endian_handler;
public: 

    explicit TIFFHeader() : _stream(std::cin) {}
    explicit TIFFHeader(std::basic_istream<char>& stream) : _stream(stream) {}


    template<unsigned int N>
    bool read(std::array<char, N>& buffer) {
        this->_stream.read(buffer.data(), buffer.size());

        if(!this->_stream) {
            return 0;
        }

        return 1;
    }

    std::shared_ptr<VirtualEndianHandler> get_endian_handler() {
        return this->endian_handler;
    }

    uint_t get_idf_offset() {
        return this->idf_offset;
    }

    void parse_header() {
        //endan
        this->err = read<2>(endian);

        std::cout << endian[0] << " 1 " << endian[1] << "\n";

        if(endian[0] == 'I' && endian[1] == 'I') {
            std::cout << "Little endian \n";
            this->endian_handler = std::make_shared<LittleEndian_TIFF>();

        } else if(endian[0] == 'M' && endian[1] == 'M') {
            std::cout << "Big endian \n";
        } else {
            std::cout << "Error got " << this->endian[0] << " " << this->endian[1] << "\n"; 
        }

        std::array<char, 2> arr_magic_number{0, 0};
        read<2>(arr_magic_number);
        std::cout << "got " << std::format("{:02X} {:02X}\n", static_cast<unsigned char>(arr_magic_number[0]), static_cast<unsigned char>(arr_magic_number[1]) );
        this->magic_num = this->endian_handler->convert(arr_magic_number);

        std::cout << "magic num " << this->magic_num << "\n";

        std::array<char, 4> ifd_arr;
        read<4>(ifd_arr);

        this->idf_offset = this->endian_handler->convert(ifd_arr);

        std::cout << "idf_offset " << this->idf_offset << "\n";

    }


};
