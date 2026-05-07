#include "FileReader.hpp"


FileReader& FileReader::seekg (std::streampos pos) {
    this->_fstream.seekg(pos);
    return *this;
}

std::ifstream::pos_type FileReader::tellg() {
    return this->_fstream.tellg();
}

int FileReader::sync() {
    return this->_fstream.sync();
}

FileReader& FileReader::read( std::ifstream::char_type* s, std::streamsize count ) {
    this->_fstream.read(s, count);
    return *this;
}

bool FileReader::good() const {
    return _fstream.good();
}

bool FileReader::eof() const {
    return _fstream.eof();
}

bool FileReader::fail() const {
    return _fstream.fail();
}

bool FileReader::bad() const {
    return _fstream.bad();
}

FileReader::operator bool() const noexcept {
    return static_cast<bool>(_fstream);
}