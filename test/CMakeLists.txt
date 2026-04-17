cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Google Test
find_package(OpenCV REQUIRED)

#Add libtiff
find_package(TIFF REQUIRED)

add_executable(
  hello_test
  src/tests/test_read.cc
  src/tests/test_rgb.cpp
  src/tests/test_metadata.cpp
  src/tests/test_ramp_image.cpp
)

target_compile_features(hello_test PUBLIC cxx_std_20)

target_compile_options(hello_test PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wswitch>
)

target_link_directories(hello_test PRIVATE "C:/Users/user/AppData/Local/Programs/Python/Python312/libs")
target_include_directories(hello_test PRIVATE
  ${opencv_SOURCE_DIR}/include
  ${opencv_BINARY_DIR}           # for opencv2/opencv_modules.hpp, etc.
)

target_include_directories(hello_test PRIVATE
        ${OPENCV_CONFIG_FILE_INCLUDE_DIR}
        ${OPENCV_MODULE_opencv_core_LOCATION}/include
        ${OPENCV_MODULE_opencv_imgcodecs_LOCATION}/include
        ${OPENCV_MODULE_opencv_imgproc_LOCATION}/include
        )

target_link_libraries(
  hello_test
  tiffLib
  opencv_core
  opencv_imgproc
  opencv_imgcodecs
  GTest::gtest_main
  TIFF::TIFF
)

target_compile_definitions(hello_test PRIVATE TEST_DATA_DIR="${CMAKE_SOURCE_DIR}")


include(GoogleTest)
gtest_discover_tests(hello_test)

message("${opencv_SOURCE_DIR}")
message("${opencv_BINARY_DIR}")
