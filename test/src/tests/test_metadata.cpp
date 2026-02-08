#include <gtest/gtest.h>

#include <filesystem>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "tiff-writer.hpp"
#include "TestRGBImages.hpp"

#include <tiffio.h>
#include <any>
#include <list>
#include <tuple>


TEST(MetaT, BasicAssertions) {

  size_t rows = 10;
  size_t columns = 7;

  TestImageRampRGB im(rows, columns, RampType::HORIZONTAL, 0, 255, {1,0,0}); 
  im.generateImage();

  std::string filename = "rgb-ramp-tifflib.tif";

  ImageContainer<uint8_t> img_ptr(0, 0);
  std::filesystem::path test_path;
//   std::tie(img_ptr, test_path) = write_image_and_read(filename, im.get_image());
  // ImageContainer<uint8_t> img_ptr = write_image_and_read(filename, im.get_image(), test_path);
  
    tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, im.get_image().get(), TiffCompression::PackBits);
    // tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, im.get_image().get(), TiffCompression::None);
    tiff_writer.write();


    TIFF* tif = TIFFOpen(filename.c_str(), "r");
    if (!tif) {
        throw std::runtime_error("Failed to open TIFF file: ");
    }

    uint32_t width, height;
    uint16_t bitsPerSample, samplesPerPixel, photometric;
    uint16_t compression, orientation, resUnit;
    float xRes, yRes;
    char *description, *software, *dateTime, *artist;

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH,      &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH,     &height);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE,   &bitsPerSample);
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
    TIFFGetField(tif, TIFFTAG_PHOTOMETRIC,     &photometric);
    TIFFGetField(tif, TIFFTAG_COMPRESSION,     &compression);
    TIFFGetField(tif, TIFFTAG_ORIENTATION,     &orientation);
    TIFFGetField(tif, TIFFTAG_XRESOLUTION,     &xRes);
    TIFFGetField(tif, TIFFTAG_YRESOLUTION,     &yRes);
    TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT,  &resUnit);

    std::cout << "width " << width << "\n";
    std::cout << "height " << height << "\n";
    std::cout << "bitsPerSample " << bitsPerSample << "\n";
    std::cout << "samplesPerPixel " << samplesPerPixel << "\n";
    std::cout << "photometric " << photometric << "\n";
    std::cout << "compression " << compression << "\n";
    std::cout << "orientation " << orientation << "\n";

    tsize_t tileSize = TIFFTileSize(tif);
    std::vector<uint8_t> tileBuf(tileSize);

    std::vector<uint32_t> rgba(width * height);
    if (!TIFFReadRGBAImageOriented(tif,
            width, height,
            rgba.data(),
            ORIENTATION_TOPLEFT, 0)) {
        throw std::runtime_error("Failed to read image data");
    }

    TIFFClose(tif);

    auto images = im.get_image();

    for (size_t i = 0; i < rgba.size(); i++) {
    // for (size_t i = 0; i < 5; i++) {
        uint8_t r = TIFFGetR(rgba[i]);
        uint8_t g = TIFFGetG(rgba[i]);
        uint8_t b = TIFFGetB(rgba[i]);

        size_t row = i / width;   // = i / 7
        size_t col = i % width;   // = i % 7


        std::cout << "i " << i << " row " << row << " col " << col << "\n";
        std::cout << std::format("TIFFLIB r {} g {} b {}", r, g, b) << "\n";

        std::cout << std::format("Image r {} g {} b {}",
            im.get_image()->at(row, col, 0),
            im.get_image()->at(row, col, 1),
            im.get_image()->at(row, col, 2)) << std::endl;


        EXPECT_EQ(r, im.get_image()->at(row, col, 0)) << r << " does not match " << im.get_image()->at(row, col, 0);
        EXPECT_EQ(g, im.get_image()->at(row, col, 1)) << g << " does not match " << im.get_image()->at(row, col, 1);
        EXPECT_EQ(b, im.get_image()->at(row, col, 2)) << b << " does not match " << im.get_image()->at(row, col, 2);
    }

};


// INSTANTIATE_TEST_SUITE_P(
//     CompressionSuite,
//     RampTB,
//     ::testing::Combine(
//         ::testing::Values(6, 20, 100),             // width
//         ::testing::Values(5, 20, 100),         // height
//         ::testing::Values(1, 2),       // Color pixel
//         ::testing::Values(0, 10),       // Color min level
//         ::testing::Values(250)       // Color max level
//     )
// );