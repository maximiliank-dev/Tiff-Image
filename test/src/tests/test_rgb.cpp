#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <memory>
#include <filesystem>
#include <tuple>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "tiff-writer.hpp"
#include "TestRGBImages.hpp"

struct Config {
    size_t rows;
    size_t cols;
    size_t px_position;
    size_t color;
};

class SingleColorTB : public testing::TestWithParam<std::tuple<size_t, size_t, size_t, uint8_t>> {
protected:
  size_t get_rows() {
    return std::get<0>(GetParam());
  }
  size_t get_cols() {
    return std::get<1>(GetParam());
  }
  size_t get_px_position() {
    return std::get<2>(GetParam());
  }
  uint8_t get_color() {
    return std::get<3>(GetParam());
  }

  std::string get_file_extension() {
    return "row-";
    // return "row-" + to_string(get_rows()) + "col-" + to_string(get_cols()) + "px-" + to_string(get_px_position());
      // + "color-" + to_string(get_color());
  }
};

// void testAssertions(TestImageSingleColor im) {
//   im.generateImage();

//   std::string filename = "img23.tif";
//   TiffWriter tiff_writer = TiffWriter(filename, SupportedImageTypes::RGB, im.get_image());
//   tiff_writer.write();

//   std::cout << "fff \n";
//   const std::filesystem::path test_path = std::filesystem::path(TEST_DATA_DIR) / "strips-red.tif";
//   std::cout << "test_path " << test_path.string() << "\n";

//   std::ifstream file(test_path, std::ios::binary);
//   ASSERT_TRUE(file.good()) << "Could not open: " << test_path.string();


//   // ASSERT_TRUE(0);

//   // image is BGR by default
//   cv::Mat M_brg = cv::imread(test_path.string(), cv::IMREAD_COLOR);
//   ASSERT_FALSE(M_brg.empty()) << "OpenCV could not read: " << test_path.string();

//   cv::Mat M;
//   cv::cvtColor(M_brg, M, cv::COLOR_BGR2RGB);

//   TIFFHeader header(file);

//   header.parse_header();

//   TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
//   ifd.read();
//   TiffReadStrips strips(ifd, ifd.get_endian_handler());
//   ImageContainer<uint8_t> img_ptr = strips.getImage();

//   std::cout << "CV rows " << M.rows << "\n";
//   std::cout << "CV cols " << M.cols << "\n";
//   std::cout << "M type " << M.type() << std::endl;
//   std::cout << "M channels " << M.channels() << std::endl;

//   EXPECT_EQ(M.channels(), 3);
//   EXPECT_EQ(M.rows, img_ptr.get_height());
//   EXPECT_EQ(M.cols, img_ptr.get_width());

//   for(int i = 0; i < 3; i++) {
//     for(int j = 0; j < M.cols; j++) {
//       cv::Vec3b el = M.at<cv::Vec3b>(i, j);
//       for(int k = 0; k < 3; k++) {
//         uint8_t M_el = el[k];
//         uint8_t I_el = img_ptr.at(i,j,k);

//         std::cout << "i " << i << " j " << j << " k " << k << " M " << M_el << " img " << I_el << std::endl; 
//         EXPECT_EQ(M_el, I_el);
//         if()
//         EXPECT_EQ(M_el, I_el);
//       }
//     }
//   }

// }

// Demonstrate some basic assertions.
TEST_P(SingleColorTB, BasicAssertions) {

  TestImageSingleColor im(7, 6, get_px_position(), get_color()); 
  im.generateImage();

  std::string filename = "imgg"+get_file_extension()+".tif";
  TiffWriter tiff_writer = TiffWriter(filename, SupportedImageTypes::RGB, im.get_image());
  tiff_writer.write();

  std::cout << "fff \n";
  // const std::filesystem::path test_path = std::filesystem::path(TEST_DATA_DIR) / "strips-red.tif";
  const std::filesystem::path test_path = filename;
  // const std::filesystem::path test_path = "test-red.tif";
  std::cout << "test_path " << test_path.string() << "\n";

  std::ifstream file(test_path, std::ios::binary);
  ASSERT_TRUE(file.good()) << "Could not open: " << test_path.string();


  // ASSERT_TRUE(0);


  TIFFHeader header(file);

  header.parse_header();

  TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
  ifd.read();
  TiffReadStrips strips(ifd, ifd.get_endian_handler());
  ImageContainer<uint8_t> img_ptr = strips.getImage();


  // image is BGR by default
  cv::Mat M_brg = cv::imread(test_path.string(), cv::IMREAD_COLOR);
  ASSERT_FALSE(M_brg.empty()) << "OpenCV could not read: " << test_path.string();

  cv::Mat M;
  cv::cvtColor(M_brg, M, cv::COLOR_BGR2RGB);


  std::cout << "CV rows " << M.rows << "\n";
  std::cout << "CV cols " << M.cols << "\n";
  std::cout << "M type " << M.type() << std::endl;
  std::cout << "M channels " << M.channels() << std::endl;

  EXPECT_EQ(M.channels(), 3);
  EXPECT_EQ(M.rows, img_ptr.get_height());
  EXPECT_EQ(M.cols, img_ptr.get_width());

  for(size_t i = 0; i < 3; i++) {
    for(size_t j = 0; j < M.cols; j++) {
      cv::Vec3b el = M.at<cv::Vec3b>(i, j);
      for(size_t k = 0; k < 3; k++) {
        uint8_t M_el = el[k];
        uint8_t I_el = img_ptr.at(i,j,k);

        std::cout << "i " << i << " j " << j << " k " << k << " M " << 
          static_cast<int>(M_el) << " img " << 
          static_cast<int>(I_el) << std::endl; 
        EXPECT_EQ(M_el, I_el);
        // if( k == get_px_position() ) {
        //   EXPECT_EQ(I_el, get_color()) << "Color is not " << get_color() << " at pixel position " << get_px_position();
        // }
      }
    }
  }
};


INSTANTIATE_TEST_SUITE_P(
    CompressionSuite,
    SingleColorTB,
    ::testing::Combine(
        ::testing::Values(6),             // width
        ::testing::Values(5, 20),         // height
        ::testing::Values(0, 1, 2),       // Color pixel
        ::testing::Values(250, 125)       // Color levels
    )
);