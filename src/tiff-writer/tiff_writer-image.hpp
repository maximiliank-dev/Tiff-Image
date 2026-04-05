#pragma once

#include "tiff-writer-header.hpp"

namespace tifflib {


enum class SupportedImageTypes { Bitlevel, Grayscale, RGB};
using TiffWriterImgType = TiffWriteData<uint8_t>;

/**
 * Class configuration for Binary Images
 */
class BitlevelImage : public TiffWriterImgType {

public:
    BitlevelImage(TiffWriterHeader header, const ImageContainer<uint8_t>* img, std::basic_ostream<char>& stream) : TiffWriterImgType(header, img, stream) { }

    void tags_to_set() {
        TiffWriterImgType::tags_to_set();
        
        this->_values[TiffTagType::XResolution] = make_variant(TiffTagType::XResolution, 122333);
        this->_values[TiffTagType::YResolution] = make_variant(TiffTagType::YResolution, 133333);
        this->_values[TiffTagType::ResolutionUnit] = make_variant(TiffTagType::ResolutionUnit, 2);
        // set to unsigned type
        this->_values[TiffTagType::SampleFormat] = make_variant(TiffTagType::SampleFormat, 1);
        // set to unsigned type
        this->_values[TiffTagType::Orientation] = make_variant(TiffTagType::Orientation, 1);
    }
};

/**
 * Class configuration for Grey Images
 * Data are 8 Bits long
 * White is 0xFF, Black is 0x0
 */
class GrayImage : public BitlevelImage {

public:
    GrayImage(TiffWriterHeader header, const ImageContainer<uint8_t>* img, std::basic_ostream<char>& stream) : BitlevelImage(header, img, stream) { }

    void tags_to_set() {
        BitlevelImage::tags_to_set();        

        this->_values[TiffTagType::BitsPerSample] = make_variant(TiffTagType::BitsPerSample, 8);
    }
};

/**
 * Class configuration for RGB Images
 * Data are 8 Bits long
 * Max color is 0xFF, min color is 0x0
 */
class RGBImage : public BitlevelImage {

public:
    RGBImage(TiffWriterHeader header, const ImageContainer<uint8_t>* img, std::basic_ostream<char>& stream) : BitlevelImage(header, img, stream) 
    { 
        if(this->_img->get_pixel_number_of_colors() != 3) {
            throw std::runtime_error("Error img must have 3 pixels per color");
        }

        if(this->_img->get_total_size() % 3 != 0) {
            throw std::runtime_error("Error img size must be dividable by 3");
        }
    }

    void tags_to_set() {
        BitlevelImage::tags_to_set();

        this->_values[TiffTagType::SamplesPerPixel] = make_variant(TiffTagType::SamplesPerPixel, 3 );
        //pixel are stored in order
        this->_values[TiffTagType::PlanarConfiguration] = make_variant(TiffTagType::PlanarConfiguration, 1 );
        this->_values[TiffTagType::PhotometricInterpretation] =  make_variant(TiffTagType::PhotometricInterpretation, 2 );
        this->_values[TiffTagType::BitsPerSample] = make_variant(TiffTagType::BitsPerSample, {8, 8, 8});
        this->_values[TiffTagType::SampleFormat] = make_variant(TiffTagType::SampleFormat, {1, 1, 1});

    }
};

};
