#pragma once

#include <filesystem>
#include <fstream>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "../ImageContainer.hpp"

class TiffReader {
    std::unique_ptr<TIFFHeader> _header;
    std::unique_ptr<TiffIFD> _ifds;
    std::unique_ptr<TiffReadStrips> _strips;

    std::ifstream _file; 

    bool _data_valid;

public:
    explicit TiffReader(std::filesystem::path image_path, std::shared_ptr<ImageContainer<uint8_t>> img ) : _data_valid(false)
    {
        this->_file = std::ifstream(image_path, std::ios::binary);
        if(!this->_file) {
            throw std::runtime_error(std::format("Error, could not access file in path {}\n", 22));
            // throw std::runtime_error(std::format("Error, could not access file in path {}\n", image_path));
        }

        this->_header = std::unique_ptr<TIFFHeader>(new TIFFHeader(this->_file ) );
    }

    void read() {
        this->_header->parse_header();

        // this->_ifds(this->file, this->_header.get_idf_offset(), this->_header.get_endian_handler());
        // this->ifds.read();
        // this->_strips(this->ifds, this->ifds.get_endian_handler());
    }

    ImageContainer<uint8_t> get_image() {
        if(this->_data_valid) {
            return this->_strips->get_image();
        }

        return ImageContainer<uint8_t>(0, 0);
    }
};