#pragma once

#include <format>
#include<filesystem>
#include <tuple>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "tiff-writer.hpp"
#include "tiff-reader.hpp"


inline std::tuple<ImageContainer<uint8_t>, std::filesystem::path> write_image_and_read(std::string filename, 
    std::shared_ptr<ImageContainer<uint8_t>> img
    ) {

    tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, img);
    tiff_writer.write();

    const std::filesystem::path test_path = filename;
    std::cout << "test_path " << test_path.string() << "\n";

    std::ifstream file(test_path, std::ios::binary);
    if(!file.good()) {
        throw std::runtime_error(std::format("Could not open: {}", test_path.string()));
    }

    tifflib::TIFFHeader header(file);
    header.parse_header();

    tifflib::TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
    ifd.read();
    tifflib::TiffReadStrips strips(ifd, ifd.get_endian_handler());
    ImageContainer<uint8_t> img_ptr = strips.get_image();

    return std::make_tuple(std::move(img_ptr), test_path);
}
