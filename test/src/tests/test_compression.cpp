#include <gtest/gtest.h>

#include <filesystem>

#include "compression/lzw.hpp"
#include "compression/packbits.hpp"
#include "TestRGBImages.hpp"
#include "../test-helpers/test-init-methods.hpp"
#include "../test-helpers/libtiff_wrapper.hpp"

#include <tiffio.h>
#include <list>
#include <tuple>

struct TestParameter {
    std::vector<uint8_t> data;
    std::vector<uint16_t> data_compressed;
};

class LZWTest :
    public testing::TestWithParam<TestParameter> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.

};

TEST_P(LZWTest, LZWBasic) {

    compression::LZW lzw;

    std::vector<uint8_t> data = GetParam().data;
    std::vector<uint16_t> data_compressed = GetParam().data_compressed;


    std::vector<uint16_t> comp = lzw.compress(data);
    for(size_t i = 0; i < comp.size(); ++i) {
        uint16_t element = comp.at(i);
        if( element < 256) {
            std::cout << static_cast<char>(element);
        } else
            std::cout << i;
    
        std::cout << " ";
        EXPECT_EQ(element, data_compressed.at(i) )  << "Error compression expected " << data_compressed.at(i) << " got " << element << '\n';
    }
    std::cout << "\n\n";  


    std::vector<uint8_t> decomp = lzw.decompress(data_compressed);
    for(size_t i = 0; i < decomp.size(); ++i) {
        uint8_t element = decomp.at(i);
        if( element < 256) {
            std::cout << static_cast<char>(element);
        } else
            std::cout << i;
    
        std::cout << " ";
        EXPECT_EQ(element, data.at(i) )  << "\nError decompression expected " << data.at(i) << " got " << element << " i " << i << '\n';
    }   

};

TEST_P(LZWTest, LZWTiff) {

    compression::LZW lzw;

    std::vector<uint8_t> input = GetParam().data;

    auto mine    = lzw.compress_tiff(input);
    auto theirs  = libtiff_wrapper::libtiff_lzw_encode(input);

    auto decode_theirs= libtiff_wrapper::libtiff_lzw_decode(mine, static_cast<uint32_t>(input.size()));
    auto decode_mine = lzw.decompress_tiff(theirs);

    std::cout << "Compare both decoders\n";
    for(size_t i = 0; i < decode_mine.size(); i++ ) {
        std::cout << decode_mine.at(i) << " " << decode_theirs.at(i) << "\n";

        EXPECT_EQ(decode_mine.at(i), decode_theirs.at(i));
    }


    std::cout << "Compare my decoder to the ground truth\n";
    for(size_t i = 0; i < decode_mine.size(); i++ ) {
        std::cout << decode_mine.at(i) << " " << input.at(i) << "\n";

        EXPECT_EQ(decode_mine.at(i), input.at(i));
    }


    std::cout << "Compare my self encoded and decoded result to the ground truth\n";
    auto decode_mine_mine = lzw.decompress_tiff(mine);
    for(size_t i = 0; i < decode_mine.size(); i++ ) {
        std::cout << decode_mine_mine.at(i) << " " << input.at(i) << "\n";

        EXPECT_EQ(decode_mine_mine.at(i), input.at(i));
    }
};

INSTANTIATE_TEST_SUITE_P(BasicTest,
                         LZWTest,
                         testing::Values(TestParameter{
                            {'L','Z','W','L','Z','7','8','L','Z','7','7','L','Z','C','L','Z','M','W','L','Z','A','P'},
                            {'L', 'Z', 'W', 256, '7', '8', 259, '7', 256, 'C', 256, 'M', 258, 'Z', 'A', 'P'}
                         },
                        TestParameter{
                            {'a', 'a', 'a', 'a'},
                            {'a', 256, 'a'}
                         },
                         TestParameter{
                            {'a', 'b', 'a', 'b', 'a', 'b', 'a'},
                            {'a', 'b', 256, 258}
                        },
                        TestParameter{
                            {'a', 'b', 'c', 'd'},
                            {'a', 'b', 'c', 'd'}
                        },
                        TestParameter{
                            {'a'},
                            {'a'}
                        }
                        ));

struct RampCompressionParam {
    size_t rows;
    size_t cols;
    size_t px_position;
    uint8_t color_min;
    uint8_t color_max;
};

class RampRGBCompressionTest : public testing::TestWithParam<RampCompressionParam> {};

// TEST_P(RampRGBCompressionTest, PackBits) {
//     auto p = GetParam();
//     TestImageRampRGB im(p.rows, p.cols, RampType::HORIZONTAL, p.color_min, p.color_max, {p.px_position});
//     im.generateImage();

//     std::vector<uint8_t> data = im.get_image()->get_data();

//     std::vector<uint8_t> compressed = compression::PackBits::compress(data);
//     std::vector<uint8_t> decompressed = compression::PackBits::decompress(compressed);

//     ASSERT_EQ(data.size(), decompressed.size());
//     for (size_t i = 0; i < data.size(); ++i) {
//         EXPECT_EQ(data[i], decompressed[i]) << "Mismatch at index " << i;
//     }
// }

TEST_P(RampRGBCompressionTest, LZW) {
    auto p = GetParam();
    TestImageRampRGB im(p.rows, p.cols, RampType::HORIZONTAL, p.color_min, p.color_max, {p.px_position});
    im.generateImage();

    const std::vector<uint8_t>& data = im.get_image()->get_data();

    // 1. Pure algorithm round-trip
    std::vector<uint8_t> data_copy(data);
    std::vector<uint16_t> compressed = compression::LZW::compress(data_copy);
    std::vector<uint8_t> decompressed = compression::LZW::decompress(compressed);


    ASSERT_EQ(data.size(), decompressed.size());
    for (size_t i = 0; i < data.size(); ++i) {
        EXPECT_EQ(data[i], decompressed[i]) << "Algorithm round-trip mismatch at index " << i;
    }

    // // 2. Tifflib round-trip: write with LZW, read back, verify against original
    std::string filename = "lzw-ramp-" + std::to_string(p.rows) + "-"
        + std::to_string(p.cols) + "-" + std::to_string(p.px_position) + ".tif";

    ImageContainer<uint8_t> img_read(0, 0);
    // std::filesystem::path tiff_path;
    // std::tie(img_read, tiff_path) = write_image_and_read(
    //     filename, im.get_image(), tiff_header_endian::LITTLE, TiffCompression::LZW);

    const std::vector<uint8_t>& read_data = img_read.get_data();

    std::cout << "filename " << filename << std::endl;
    //Read file using Tiffio

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
    std::cout << "compression " << compression << "\n";

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


    ASSERT_EQ(data.size(), read_data.size()) << "Tifflib read size mismatch";
    for (size_t i = 0; i < data.size(); ++i) {
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


        // EXPECT_EQ(r, im.get_image()->at(row, col, 0)) << r << " does not match " << im.get_image()->at(row, col, 0);
        // EXPECT_EQ(g, im.get_image()->at(row, col, 1)) << g << " does not match " << im.get_image()->at(row, col, 1);
        // EXPECT_EQ(b, im.get_image()->at(row, col, 2)) << b << " does not match " << im.get_image()->at(row, col, 2);
    }
}

INSTANTIATE_TEST_SUITE_P(
    RampRGBSuite,
    RampRGBCompressionTest,
    testing::Values(
        RampCompressionParam{6,   5,   1, 0,  250}
        // RampCompressionParam{20,  20,  2, 10, 250},
        // RampCompressionParam{100, 100, 1, 0,  250}
    )
);