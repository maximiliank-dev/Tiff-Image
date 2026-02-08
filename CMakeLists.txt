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

#enable warning for the switch statements covering enum classes
if (MSVC)
    target_compile_options(tiffLib PRIVATE /w14062)
else()
    target_compile_options(tiffLib PRIVATE -Wswitch)
endif()

target_compile_definitions(tiffLib PUBLIC PRINT=0)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)
add_subdirectory(test)

add_executable(my_app src/main.cpp )
target_link_libraries(my_app PUBLIC tiffLib)


#Sphix build
find_program(SPHINX_EXECUTABLE sphinx-build
    HINTS $ENV{SPHINX_DIR}
    DOC "Sphinx documentation generator"
)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/doxygen/xml" DOXYGEN_XML_PATH)

message("Sphinx ${SPHINX_EXECUTABLE}")
if(SPHINX_EXECUTABLE)
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs/source)
    set(SPHINX_BUILD  ${CMAKE_CURRENT_BINARY_DIR}/docs)

    add_custom_target(docs
        COMMAND ${SPHINX_EXECUTABLE}
            -b html
            ${SPHINX_SOURCE}
            ${SPHINX_BUILD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Sphinx"
        VERBATIM
    )
endif()