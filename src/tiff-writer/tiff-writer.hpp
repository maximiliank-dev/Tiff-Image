#pragma once

#include <filesystem>
#include <string>
#include <format>
#include <memory>

#include "../io/FileReader.hpp"
#include "tiff-writer-header.hpp"
#include "tiff_writer-image.hpp"



class TiffWriter {

    std::ofstream _file;
    std::unique_ptr<TiffWriterHeader> _header;
    std::unique_ptr<TiffWriteData<uint8_t>> _writer;

public:
    explicit TiffWriter(std::filesystem::path image_path, SupportedImageTypes type, std::shared_ptr<ImageContainer<uint8_t>> img ) {

        this->_file = std::ofstream(image_path, std::ios::binary);
        if(!this->_file) {
            throw std::runtime_error(std::format("Error, could not access file in path {}\n", 22));
            // throw std::runtime_error(std::format("Error, could not access file in path {}\n", image_path));
        }

        this->_header = std::unique_ptr<TiffWriterHeader>(new TiffWriterHeader(this->_file ) );

        switch(type) {
            case SupportedImageTypes::Bitlevel:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(new BitlevelImage(*(this->_header), img, this->_file) );
                break;
            case SupportedImageTypes::Grayscale:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(new GrayImage(*(this->_header), img, this->_file) );
                break;
            case SupportedImageTypes::RGB:
                this->_writer = std::unique_ptr<TiffWriteData<uint8_t>>(new RGBImage(*(this->_header), img, this->_file) );
                break;
        }

    };

    void write() {
        this->_header->write_header();   
        this->_writer->write();
    };
};