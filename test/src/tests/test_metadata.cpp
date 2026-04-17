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
  
    // tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, im.get_image().get(), TiffCompression::PackBits);
    tifflib::TiffWriter tiff_writer(filename, tifflib::SupportedImageTypes::RGB, im.get_image().get(), TiffCompression::None);
    tiff_writer.write();


    TIFF* tif = TIFFOpen(filename.c_str(), "r");
    if (!tif) {
        throw std::runtime_error("Failed to open TIFF file: ");
    }


    std::list<std::tuple<std::any, std::string>> data;

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
            ORIENTATION_TOPLEFT, 0))
        throw std::runtime_error("Failed to read image data");


    // uint32_t bytesPerPixel = (img.bitsPerSample / 8) * img.samplesPerPixel;
    // // img.data.resize(img.width * img.height * bytesPerPixel, 0);

    // for (uint32_t y = 0; y < height; y += tileHeight) {
    //     for (uint32_t x = 0; x < width; x += tileWidth) {
    //         if (TIFFReadTile(tif, tileBuf.data(), x, y, 0, 0) < 0)
    //             throw std::runtime_error("Failed to read tile");

    //         // Copy tile into image buffer row by row
    //         for (uint32_t row = 0; row < tileHeight && (y + row) < img.height; row++) {
    //             uint32_t cols = std::min(tileWidth, img.width - x);
    //             uint8_t* dst = img.data.data() + ((y + row) * img.width + x) * bytesPerPixel;
    //             uint8_t* src = tileBuf.data() + row * tileWidth * bytesPerPixel;
    //             // std::memcpy(dst, src, cols * bytesPerPixel);
    //             for(size_t i = 0; i < cols; i++) {
    //                 CHECK
    //             }
    //         }
    //     }
    // }




    TIFFClose(tif);

    auto images = im.get_image();

    for (size_t i = 0; i < rgba.size(); i++) {
    // for (size_t i = 0; i < 5; i++) {
        uint8_t r = TIFFGetR(rgba[i]);
        uint8_t g = TIFFGetG(rgba[i]);
        uint8_t b = TIFFGetB(rgba[i]);

        size_t j = i % width;
        size_t k = (i / width);

        std::cout << "i " << i << " j " << j << " k " << k << "\n";
        std::cout << std::format("TIFFLIB r {} g {} b {}", r, g, b) << "\n";
        // std::cout << std::format("Image r {} g {} b {}", 
            // im.get_image()->at(j, k, 0), 
            // im.get_image()->at(j, k, 1), 
            // im.get_image()->at(j, k, 2)) << std::endl;
        size_t row = i / width;   // = i / 7
        size_t col = i % width;   // = i % 7

        std::cout << std::format("Image r {} g {} b {}",
            im.get_image()->at(row, col, 0),
            im.get_image()->at(row, col, 1),
            im.get_image()->at(row, col, 2)) << std::endl;


        // if(r != im.get_image()->at(j, k, 0))
        //     break;
        // EXPECT_EQ(r, im.get_image()->at(j, k, 0));
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