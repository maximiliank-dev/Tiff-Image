#include <iostream>
#include <fstream>
#include <array>
#include <format>
#include <memory>

#include "tiff-header.hpp"
#include "tiff-data.hpp"
#include "io/FileReader.hpp"
#include "ImageContainer.hpp"
#include "tiff-writer/tiff-writer-header.hpp"

int main() {

    std::cout << std::unitbuf;

    std::cout << "runs\n";

    std::ifstream file("../test.tif", std::ios::binary);

    std::cout << "good " << file.good() << "\n";

    // char c;
    // file >> c;
    // std::cout << "c " << c << "\n";
    //char cc[2];
    //file.read(cc, 2);
    //std::cout << "cc " << cc[0] << " " << cc[1] << "\n";


    TIFFHeader header(file);

    header.parse_header();

    TiffIFD ifd(file, header.get_idf_offset(), header.get_endian_handler());
    ifd.read();
    TiffReadStrips strips(ifd, ifd.get_endian_handler());
    strips.getImage();


    char c = 0x80; //-128
    std::cout << "int " << static_cast<int>(c) << " " << std::format("0x{:08X}", static_cast<int>(c)) << "\n";
    std::cout << "uint " << static_cast<unsigned int>(c) << " " << std::format("0x{:08X}", static_cast<unsigned int>(c)) << "\n";

    //std::cout << std::flush;

    std::cout << "convert uint " << static_cast<unsigned int>(static_cast<unsigned char>(c)) << " " << std::format("0x{:08X}", static_cast<unsigned int>(static_cast<unsigned char>(c)) ) << "\n";


    FileReader fr("test.tiff");

    std::ofstream file_out("../out.tif", std::ios::binary);


    ///////////////////////////////////////
    std::shared_ptr<ImageContainer<uint8_t>> img(new ImageContainer<uint8_t>(8, 1));
    img->append_columns<std::vector<uint8_t>>(std::vector<uint8_t>{0, 1, 2, 3, 4, 5, 6, 7});
    img->append_columns<std::vector<uint8_t>>(std::vector<uint8_t>{10, 11, 12, 13, 14, 15, 16, 17});
    img->append_columns<std::vector<uint8_t>>(std::vector<uint8_t>{20, 21, 22, 23, 24, 25, 26, 27});

    TiffWriterHeader header_writer(file_out);
    header_writer.write_header();

    TiffWriteData<uint8_t> data(header_writer, img, header_writer.get_stream());
    data.write();
    
    return 0;
}

