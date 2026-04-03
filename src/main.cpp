#include <iostream>
#include <fstream>
#include <array>
#include <format>
#include <memory>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "io/FileReader.hpp"
#include "ImageContainer.hpp"
#include "tiff-writer/tiff_writer-image.hpp"

using namespace tifflib;

int main() {

    std::cout << std::unitbuf;

    std::cout << "runs\n";

    std::ifstream file("../test.tif", std::ios::binary);

    std::cout << "good " << file.good() << "\n";

    TIFFHeader header(file);

    header.parse_header();

    TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
    ifd.read();
    TiffReadStrips strips(ifd, ifd.get_endian_handler());
    strips.get_image();


    char c = -128; //-128
    std::cout << "int " << static_cast<int>(c) << " " << std::format("0x{:08X}", static_cast<int>(c)) << "\n";
    std::cout << "uint " << static_cast<unsigned int>(c) << " " << std::format("0x{:08X}", static_cast<unsigned int>(c)) << "\n";

    //std::cout << std::flush;

    std::cout << "convert uint " << static_cast<unsigned int>(static_cast<unsigned char>(c)) << " " << std::format("0x{:08X}", static_cast<unsigned int>(static_cast<unsigned char>(c)) ) << "\n";


    FileReader fr("test.tiff");

    std::ofstream file_out("../out.tif", std::ios::binary);


    ///////////////////////////////////////
    std::shared_ptr<ImageContainer<uint8_t>> img(new ImageContainer<uint8_t>(8, 1));
    img->append_row<std::vector<uint8_t>>(std::vector<uint8_t>{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    img->append_row<std::vector<uint8_t>>(std::vector<uint8_t>{   0,    0,    0,    0,    0, 0xFF,    0,   0});
    img->append_row<std::vector<uint8_t>>(std::vector<uint8_t>{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});

    TiffWriterHeader header_writer(file_out);
    header_writer.write_header();

    GrayImage data(header_writer, img, header_writer.get_stream());
    data.write();



    
    return 0;
}

