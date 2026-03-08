#pragma once

#include<ostream>
#include<map>
#include<tuple>
#include<memory>
#include<sstream>

#include "../tiff-config/types.hpp"
#include "../tiff-config/tiff-types.hpp"

#include "../endianHandler.hpp"
#include "../ImageContainer.hpp"

/**
 * Writes the basic tiff Header
 * @Params: endian: sets ths the endianess of the image 
 *          stream: the ofstream owning the file on disk 
 */
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
    // type checking
    static_assert(std::is_integral_v<TP> && std::is_unsigned_v<TP>, "TP must be of unsigned type");

private:
    TiffTagType _width; 
    TiffTagType _length;
    TiffTagType _compression; 

protected:
    uint64_t _offset = 0x0;
    uint16_t _numerTDF = 3;
    uint_t next_offset = 0;
    uint64_t _offset_data = 0x8; // data come after the HEADER like in GIMP

    std::shared_ptr<ImageContainer<TP>> _img;
    std::basic_ostream<char>& _stream;

    using SetElement = std::pair<TiffTagType, TiffDataVariant>;

    /**
     * implement the ascending sorting for SetElement
     * Required from the TIFF spec
     */
    struct less_tiff {
        constexpr bool operator()(const TiffTagType & lhs, const TiffTagType & rhs ) const {

            return static_cast<uint64_t>(lhs) < static_cast<uint64_t>(rhs);
        }
    };

    // using SetType = std::set<SetElement, less_tiff>; 
    using SetType = std::map<TiffTagType, TiffDataVariant, less_tiff>; 

    SetType _values;
    std::shared_ptr<VirtualEndianHandler> _endian_handler;


public:
    TiffWriteData(TiffWriterHeader header, std::shared_ptr<ImageContainer<TP>> img, std::basic_ostream<char>& stream) : _img(img), 
        _offset(header.get_idf_offset()), _stream(stream) 
    {
        this->_endian_handler = header.create_endian_handler();
    }

    SetElement convert_to_SetElement(TiffTagType tag, uint64_t value) {
        TiffDataVariant variant = make_variant(tag, value);

        return std::make_tuple(tag, variant);
    }


    /**
     * Write the element to the tiff file
     * modifies the offset field of the class
     * Params: element: the TiffTag
     *         rational_values: data structure to store the values of ags with data type RATIONAL
     */
    void write_ifd_element(TiffTagType tag, TiffDataVariant variant, std::vector<uint64_t>& rational_values) {
            TiffDataType type = get_tag_data_type(tag);

            // write the IFD
            //tag
            ushort_t tag_short = static_cast<ushort_t>(tag); 
            std::array<char, 2> tag_array = this->_endian_handler->convert_to_array(tag_short);
            write_char<2>(tag_array, this->_stream);

            //data type
            std::array<char, 2> type_array = this->_endian_handler->convert_to_array(static_cast<ushort_t>(type));
            write_char<2>(type_array, this->_stream);

            //count
            std::array<char, 4> tag_count = this->_endian_handler->convert_to_array(static_cast<uint_t>(1));
            write_char<4>(tag_count, this->_stream);

            std::cout << "tag " <<  int(tag) << " " << to_string(tag);

            // value (in offset)
            std::visit([this, &rational_values](auto& el) -> void {
                using T = std::decay_t<decltype(el)>;

                // std::string must be handles separate
                if constexpr ( std::is_same<T, std::string>::value) {
                    throw std::runtime_error("string not implemented yet");
                } else if constexpr(std::is_same<T, uint8_t>::value ||
                    std::is_same<T, ushort_t>::value ||
                    std::is_same<T, uint_t>::value ) { //use the implemented array conversion function
                    std::cout <<  " value " << el;
                    // convert the value to int
                    uint_t offset_value = static_cast<uint_t>(el);
                    std::array<char, 4> array = this->_endian_handler->convert_to_array(offset_value);
                    write_char<4>(array, this->_stream);
                } else if constexpr(std::is_same<T, uint64_t>::value) { //uint64_t can not be stored in the offset
                    rational_values.push_back(el);
                    std::array<char, 4> array = this->_endian_handler->convert_to_array(
                        static_cast<uint_t>(this->_offset) );
                    write_char<4>(array, this->_stream);

                    std::cout <<  " offset " << this->_offset << " el " << el;
                    this->_offset += 8;
                } else {
                    throw std::runtime_error("Type not implemented");
                }
            }, variant);
            std::cout << "\n";
    }

    /**
     * Write all IDF
     */
    void writeIDFs() {
        //setup the set of TIFF IFD entries
        this->tags_to_set();

        //backup the beginning of the IFD 
        std::streampos offset_tiff_data = this->_stream.tellp();

        std::vector<uint64_t> rational_values;
        const size_t ifd_cnt = this->_values.size() + 1;


        // add the number of bytes of the IFD size STRIP offset and StripByteCount
        // also add the "Next IFD offset"(4 Bytes) byte count and the Number of "Directory Entries"(2 bytes)
        // unit is in 'number of bytes'
        // Formula: start position + (number of ifd - 1)*Tag Size + (Directory Entries) + NextIFD
        this->_offset = static_cast<uint64_t>(offset_tiff_data) + (ifd_cnt+1-1) * TiffTag_size +2+4; //182 = 32 + 12*12 + 2 + 4

        // write the IDF count to the file
        std::array<char, 2 > tmp = this->_endian_handler->convert_to_array(
            static_cast<ushort_t>(ifd_cnt) );
        write_char<2>( tmp, this->_stream );

        // add the offset of the image data StripOffsets
        this->_values.emplace( TiffTagType::StripOffsets, make_variant(TiffTagType::StripOffsets, this->_offset_data) );

        // write the rest
        for(auto it = this->_values.begin(); it != this->_values.end(); it++ ) {
            this->write_ifd_element(it->first, it->second, rational_values);
        }

        // write the next IDF offset (is 0 in this case)
        std::array<char, 4 > tmp4 = this->_endian_handler->convert_to_array(uint_t{0});
        write_char<4>( tmp4, this->_stream );

        for(auto el : rational_values) {
            std::array<char, 8 > tmp8 = this->_endian_handler->convert_to_array(el);
            write_char<8>( tmp8, this->_stream );
        }

        // set the pointer to the IFD Offset in the header
        this->_stream.seekp(0x4);
        std::array<char, 4 > offset_idf_header = this->_endian_handler->convert_to_array(static_cast<uint_t>(offset_tiff_data));
        write_char<4>(offset_idf_header, this->_stream);
        this->_stream.seekp(std::ios::app);
    } 
    
    /**
     * Writes the image data to a file
    */
    void write_image_data() {

        const std::vector<TP>& data = this->_img->get_data();
        // compute the size of a row
        const size_t size_row = this->_img->get_width()+this->_img->get_pixel_number_of_colors()-1;

        constexpr TP max = std::numeric_limits<TP>::max();

        for(size_t i = 0; i < this->_img->get_height(); i++ ) {
            for(size_t j = 0; j < this->_img->get_width(); j++) {
                for(size_t k = 0; k < this->_img->get_pixel_number_of_colors(); k++) {
                    //check if the value range is inverted in a Grayscale or Bitlevel image
                    if( this->_values[TiffTagType::PhotometricInterpretation] == 
                        make_variant(TiffTagType::PhotometricInterpretation, 0x0) ) {
                        TP diff = max - data[i*size_row + j+k];
                        auto array = this->_endian_handler->convert_to_array(diff);
                        write_char(array, this->_stream);
                    } else {
                        auto array = this->_endian_handler->convert_to_array(data[i*size_row + j+k]);
                        write_char(array, this->_stream);
                    }
                }
            }
        }
    };

    virtual void tags_to_set() = 0;

    void write() {
        this->write_image_data();
        this->writeIDFs();
    };

};

/**
 * Set the basic TIFF Settings
 * Must be used in all image types
 */
template<typename TP>
void TiffWriteData<TP>::tags_to_set() {
    this->_values.clear();
    this->_values.emplace( TiffTagType::ImageWidth, make_variant(TiffTagType::ImageWidth, this->_img->get_width()) );
    this->_values.emplace( TiffTagType::ImageLength, make_variant(TiffTagType::ImageLength, this->_img->get_height()) );
    this->_values.emplace( TiffTagType::Compression, make_variant(TiffTagType::Compression, 1) );

    this->_values.emplace( TiffTagType::PhotometricInterpretation, make_variant(TiffTagType::PhotometricInterpretation, 0x0001) );
    this->_values.emplace( TiffTagType::RowsPerStrip,  make_variant(TiffTagType::RowsPerStrip, this->_img->get_height()) );
    this->_values.emplace( TiffTagType::StripByteCounts, 
                make_variant(TiffTagType::StripByteCounts, this->_img->get_bytes_per_row()) );
};



