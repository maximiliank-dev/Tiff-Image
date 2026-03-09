#pragma once
#include <gtest/gtest.h>

#include <fstream>
#include <string>
#include <memory>
#include <filesystem>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "tiff-header.hpp"
#include "tiff-data.hpp"

// Demonstrate some basic assertions.
TEST(TestRead22, BasicAssertions) {

  std::string cwd = std::filesystem::current_path().string();
  std::cout << "CWD " << cwd << "\n";

  const std::filesystem::path test_path = std::filesystem::path(TEST_DATA_DIR) / "test.tif";
  std::cout << "test_path " << test_path.string() << "\n";

  std::ifstream file(test_path, std::ios::binary);
  ASSERT_TRUE(file.good()) << "Could not open: " << test_path.string();

  // image is BGR by default
  cv::Mat M_brg = cv::imread(test_path.string(), cv::IMREAD_COLOR);
  ASSERT_FALSE(M_brg.empty()) << "OpenCV could not read: " << test_path.string();

  cv::Mat M;
  cv::cvtColor(M_brg, M, cv::COLOR_BGR2RGB);

  TIFFHeader header(file);

  header.parse_header();

  TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
  ifd.read();
  TiffReadStrips strips(ifd, ifd.get_endian_handler());
  ImageContainer<uint8_t> img_ptr = strips.getImage();

  std::cout << "CV rows " << M.rows << "\n";
  std::cout << "CV cols " << M.cols << "\n";
  std::cout << "M type " << M.type() << std::endl;
  std::cout << "M channels " << M.channels() << std::endl;

  EXPECT_EQ(M.channels(), 3);
  EXPECT_EQ(M.rows, img_ptr.get_width());
  EXPECT_EQ(M.cols, img_ptr.get_height());

  for(size_t i = 0; i < M.rows; i++) {
    for(size_t j = 0; j < M.cols; j++) {
      cv::Vec3b pixel = M.at<cv::Vec3b>(i, j);

      for(size_t k = 0; k < M.channels(); k++) {
        const uint8_t px = img_ptr.at(i,j,k);
        EXPECT_EQ(pixel[k], px);
        // std::cout << "i " << i << " j " << j << " k " << k << " " << int(pixel[k]) << " " << int(px) << "\n";
      }
    }
  }

  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
