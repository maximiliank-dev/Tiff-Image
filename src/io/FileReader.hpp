#pragma once

#include <fstream>
#include <string>
#include <stdexcept>


class FileReader {

private:
    std::ifstream _fstream;

public:
    FileReader(std::string file) : _fstream(file, std::ios::binary) {}

    FileReader& seekg (std::streampos pos);
    std::ifstream::pos_type tellg();
    int sync();
    FileReader& read( std::ifstream::char_type* s, std::streamsize count );

    bool good() const;
    bool eof() const;
    bool fail() const;
    bool bad() const;


    //operators
    explicit operator bool() const noexcept;

};
