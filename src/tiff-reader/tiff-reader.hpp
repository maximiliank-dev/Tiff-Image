#pragma once

#include <filesystem>
#include <fstream>
#include <optional>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "../ImageContainer.hpp"

namespace tifflib {

/**
 * Wrapper to read a Tiff Image file
 * the reader must get a valid path to the tiff image
 * 
 * Usage:
 *      TiffReader reader(<path>);
 *      reader.read();
 *      ImageContainer<uint8_t> image = reader.get_image();
 */
class TiffReader {
    std::optional<TIFFHeader> _header;
    std::optional<TiffIFD> _ifds;
    std::optional<TiffReadStrips> _strips;

    std::ifstream _file; 

    bool _data_valid;

public:
    explicit TiffReader(std::filesystem::path image_path ) : _data_valid(false)
    {
        this->_file = std::ifstream(image_path, std::ios::binary);
        if(!this->_file) {
            throw std::runtime_error(std::format("Error, could not access file in path {}\n", 22));
            // throw std::runtime_error(std::format("Error, could not access file in path {}\n", image_path));
        }
    }

    /**
     * Implements the reading if a tiff file
     */
    void read() {
        // 1. get the header
        this->_header.emplace(this->_file);
        this->_header->parse_header();

        // 2. read the IFD metadata
        this->_ifds.emplace(this->_file, this->_header->get_idf_offset(), this->_header->get_endian_handler());
        this->_ifds->read();

        // 3. initialize the strip class to read the image data
        this->_strips.emplace(*this->_ifds, this->_ifds->get_endian_handler());

        this->_data_valid = true;
    }

    /**
     * Reads the image data and reruns the Image
     * read() function must be called befor
     * @return The read image, or an image with 0 columns and 0 rows, if the read() function was not called.
     * @details read() must be called before
     */
    ImageContainer<uint8_t> get_image() {
        if(this->_data_valid) {
            return this->_strips->get_image();
        }

        return ImageContainer<uint8_t>(0, 0);
    }
};

};