#pragma once

#include <filesystem>
#include <format>
#include <memory>
#include <optional>
#include <string>

#include "../io/FileReader.hpp"
#include "tiff-writer-header.hpp"
#include "tiff_writer-image.hpp"

namespace tifflib {

/**
 * Write an ImageContainer to a Tiff File
 * Usage:
 *      TiffWriter writer(<path>, tifflib::SupportedImageTypes::RGB, image);
 *      writer.write();
 */
class TiffWriter {
    std::ofstream _file;
    std::optional<TiffWriterHeader> _header;
    std::unique_ptr<TiffWriteData<uint8_t>> _writer;

   public:
    /**
     * @arg image path: path to the image
     *      type: which TIFF image type should be used ( e.g. Grayscale, RGB,
     * ... ) img: Image to write
     */
    explicit TiffWriter(std::filesystem::path image_path,
                        SupportedImageTypes type,
                        const ImageContainer<uint8_t>* img) {
        this->_file = std::ofstream(image_path, std::ios::binary);
        if (!this->_file) {
            throw std::runtime_error(
                std::format("Error, could not access file in path {}\n",
                            image_path.string()));
        }

        this->_header.emplace(this->_file);

        switch (type) {
            case SupportedImageTypes::Bitlevel:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(
                    new BitlevelImage(*(this->_header), img, this->_file));
                break;
            case SupportedImageTypes::Grayscale:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(
                    new GrayImage(*(this->_header), img, this->_file));
                break;
            case SupportedImageTypes::RGB:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(
                    new RGBImage(*(this->_header), img, this->_file));
                break;
        }
    };

    /**
     * Perform the writing
     */
    void write() {
        this->_header->write_header();
        this->_writer->write();
    };
};
};  // namespace tifflib