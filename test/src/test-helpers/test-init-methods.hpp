#pragma once

#include <format>
#include<filesystem>
#include <tuple>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "tiff-writer.hpp"
#include "tiff-reader.hpp"


inline std::tuple<ImageContainer<uint8_t>, std::filesystem::path> write_image_and_read(std::string filename,
    std::shared_ptr<ImageContainer<uint8_t>> img,
    tiff_header_endian endian = tiff_header_endian::LITTLE
    ) {

    tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, img.get(), TiffCompression::None, endian);
    tiff_writer.write();

    const std::filesystem::path test_path = filename;
    std::cout << "test_path " << test_path.string() << "\n";

    std::ifstream file(test_path, std::ios::binary);
    if(!file.good()) {
        throw std::runtime_error(std::format("Could not open: {}", test_path.string()));
    }

    tifflib::TiffReader reader(test_path);
    reader.read();

    ImageContainer<uint8_t> img_ptr = reader.get_image();

    return std::make_tuple(std::move(img_ptr), test_path);
}
