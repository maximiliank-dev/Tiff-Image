cmake_minimum_required(VERSION 3.14)
project(myproject)

# C++20 for modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_COMPILE_FEATURES "-O0 -g -Wall -Wextra -Wswitch")


add_library(tiffLib STATIC )

target_sources(tiffLib PRIVATE 
    src/io/FileReader.cpp
    src/endianHandler.cpp
    src/ImageContainer.cpp
    src/ImageSolidColor.cpp
    src/image-generation/TestRGBImages.cpp
)

target_include_directories(tiffLib PUBLIC 
        src 
        src/tiff_config 
        src/io 
        src/tiff-reader 
        src/tiff-writer
        src/image-generation )

# target_compile_features(tiffLib PRIVATE cxx_std_17 cxx_std_20)

target_compile_definitions(tiffLib PRIVATE PRINT_LOGS)

enable_testing()
include(GoogleTest)
add_subdirectory(test)

add_executable(my_app src/main.cpp )
target_link_libraries(my_app PUBLIC tiffLib)



