#pragma once

#include<ostream>
#include<set>
#include<tuple>
#include<memory>
#include<sstream>

#include "../tiff-config/types.hpp"
#include "../tiff-config/tiff-types.hpp"

#include "../endianHandler.hpp"
#include "../ImageContainer.hpp"


class TiffWriterHeader {

private:
    std::basic_ostream<char>& _stream;

    //header
    ushort_t _magic_num = 42;
    tiff_header_endian _endian = tiff_header_endian::LITTLE;
    uint_t _idf_offset = 8;

public:
    TiffWriterHeader(std::basic_ostream<char>& stream) : _stream(stream) {}
    TiffWriterHeader(std::basic_ostream<char>& stream, const tiff_header_endian endian) : _endian(endian), _stream(stream) {}
    TiffWriterHeader(std::basic_ostream<char>& stream, const uint_t idf_offset) : _idf_offset(idf_offset), _stream(stream) {}
    TiffWriterHeader(std::basic_ostream<char>& stream, const tiff_header_endian endian, const uint_t idf_offset) : _endian(endian),  _idf_offset(idf_offset), _stream(stream) {}

    uint_t get_idf_offset() {
        return this->_idf_offset;
    }

    void write_header() {
        this->_stream.write(reinterpret_cast<char*>(&this->_endian), 2);
        this->_stream.write(reinterpret_cast<char*>(&this->_magic_num), 2);
        this->_stream.write(reinterpret_cast<char*>(&this->_idf_offset), 4);
    }

    std::shared_ptr<VirtualEndianHandler> create_endian_handler() {
        if(this->_endian == tiff_header_endian::LITTLE) {
                return std::make_shared<LittleEndian_TIFF>();

        }
        return std::make_shared<LittleEndian_TIFF>();
    }

    std::basic_ostream<char>& get_stream() { return this->_stream; }

};

/**
 * 
 */
template<typename TP>
class TiffWriteData {

private:
    std::shared_ptr<ImageContainer<TP>> _img;
    std::basic_ostream<char>& _stream;

    TiffTagType _width; 
    TiffTagType _length;
    TiffTagType _compression; 

    uint64_t _offset = 0xC;
    uint16_t _numerTDF = 3;
    uint_t next_offset = 0;

    using SetType = std::set<std::tuple<TiffTagType, TiffDataVariant>>;

    SetType _values;
    std::shared_ptr<VirtualEndianHandler> _endian_handler;

    //flags
    bool idfs_written = false;

public:
    TiffWriteData(TiffWriterHeader header, std::shared_ptr<ImageContainer<TP>> img, std::basic_ostream<char>& stream) : _img(img), 
        _offset(header.get_idf_offset()), _stream(stream) 
    {
        this->_endian_handler = header.create_endian_handler();
        this->_values.insert( std::make_tuple( TiffTagType::ImageWidth,  make_variant(TiffTagType::ImageWidth, img->get_width())) );
        this->_values.insert( std::make_tuple(TiffTagType::ImageLength, make_variant(TiffTagType::ImageLength, img->get_total_size())));
        this->_values.insert( std::make_tuple(TiffTagType::Compression, make_variant(TiffTagType::Compression, 0)));
    }

    void writeIDFs(uint64_t offset) {
        this->tags_to_set();

        this->_stream.seekp(offset);

        // write the IDF count
        std::array<char, 2 > tmp = this->_endian_handler->convert_to_array(this->_numerTDF);
        write_char<2>( tmp, this->_stream );

        std::cout << "writeIDFs " << this->_values.size() <<  "\n";

        // write the rest
        for(auto it = this->_values.begin(); it != this->_values.end(); it++ ) {

            TiffTagType tag;
            TiffDataVariant variant;
            // std::tie(tag, variant) = *it;
            tag = std::get<0>(*it);
            variant = std::get<1>(*it);

            std::cout << int(tag) << " " << to_string(tag) << "\n";


            std::visit([this](auto& el) -> void {
                using T = std::decay_t<decltype(el)>;
                // std::string must be handles separate
                if constexpr ( std::is_same<T, std::string>::value) {
                    write_char(el, this->_stream);
                } else if constexpr(std::is_same<T, uint8_t>::value) { //use the implemented array conversion function
                    std::array<char, 1> array = this->_endian_handler->convert_to_array(el);
                    write_char<1>(array, this->_stream);
                } else if constexpr(std::is_same<T, ushort_t>::value) { //use the implemented array conversion function
                    std::array<char, 2> array = this->_endian_handler->convert_to_array(el);
                    write_char<2>(array, this->_stream);
                } else if constexpr(std::is_same<T, uint_t>::value) { //use the implemented array conversion function
                    std::array<char, 4> array = this->_endian_handler->convert_to_array(el);
                    write_char<4>(array, this->_stream);
                } else if constexpr(std::is_same<T, uint64_t>::value) { //use the implemented array conversion function
                    std::array<char, 8> array = this->_endian_handler->convert_to_array(el);
                    write_char<8>(array, this->_stream);
                } else {
                    throw std::runtime_error("Type not implemented");
                }
            }, variant);
        }
        this->idfs_written = true;
    } 
    
    /**
     * Writes the image data to a file
    */
    void write_image_data() {
        if(!this->idfs_written) {
            throw std::runtime_error("Error writeIDFs() function must be called before");
        }

        for(const std::vector<TP>& el : this->_img->get_data() ) {
            //for
            //write_char(el, this->_stream);
        }

        this->idfs_written = false;
    };

    virtual void tags_to_set() = 0;

    void write() {
        this->writeIDFs(this->_offset);
        this->write_image_data();
    };

};

template<typename TP>
void TiffWriteData<TP>::tags_to_set() {
    // this->_values.clear();
    this->_values.emplace(std::make_tuple(TiffTagType::ImageWidth, static_cast<uint64_t>(this->_img->get_width())));
    this->_values.emplace(std::make_tuple(TiffTagType::ImageLength, static_cast<uint64_t>(this->_img->get_total_size())));
    this->_values.emplace(std::make_tuple(TiffTagType::Compression, static_cast<uint64_t>(0)));
    
};

template<typename TP>
class BitlevelImage : public TiffWriteData<uint8_t> {

    BitlevelImage(TiffWriterHeader header, std::shared_ptr<ImageContainer<TP>> img, std::basic_ostream<char>& stream) : _img(img), 
        _offset(header.get_idf_offset()), _stream(stream) 
    {
        this->_endian_handler = header.create_endian_handler();
    }

    void tags_to_set() {
        TiffWriteData<uint8_t>::tags_to_set();
        this->_values.emplace(std::make_tuple(TiffTagType::NewSubfileType, 0x00FE));
        this->_values.emplace(std::make_tuple(TiffTagType::ImageLength, static_cast<uint64_t>(this->_img->get_total_size())));
        this->_values.emplace(std::make_tuple(TiffTagType::Compression, static_cast<uint64_t>(0)));
    }
};
