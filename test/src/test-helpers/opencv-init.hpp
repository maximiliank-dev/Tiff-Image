#pragma once

#include <filesystem>
#include <format>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

inline cv::Mat get_tiff_image_opencv(const std::filesystem::path image_path) {

  // image is BGR by default
  cv::Mat M_brg = cv::imread(image_path.string(), cv::IMREAD_COLOR);
  if(M_brg.empty()) {
    throw std::runtime_error(std::format("OpenCV could not read file: {}", image_path.string()));
  }
  
  //convert to RGB
  cv::Mat M_rgb;
  cv::cvtColor(M_brg, M_rgb, cv::COLOR_BGR2RGB);

  return M_rgb;
} 
