#pragma once

#include <istream>
#include <iostream>
#include <format>
#include <array>
#include <vector>
#include <memory>
#include <variant>
#include <string>
#include <algorithm>
#include <type_traits>
#include <limits>


#include "tiff-config/types.hpp"
#include "tiff-config/tiff-types.hpp"
#include "tiff-config/tiff-data-variant.hpp"
#include "endianHandler.hpp"
#include "ImageContainer.hpp"

namespace tifflib {


struct TiffTagRead {
    TiffTagType TagID;
    std::vector<TiffDataVariant> data;
};


inline std::string read_char_str(std::basic_istream<char>& stream, const size_t length) {
    if(!stream) {
        std::cout << "Fail " << stream.rdstate() << " pos " << stream.tellg() << "\n";
        throw std::invalid_argument("File Stream is invalid");
    }
    std::string s(length, ' ');
    char* buf = s.data(); 
    stream.read(buf, s.size());

    if(!stream) {
        std::cout << "Fail " << stream.rdstate() << " pos " << stream.tellg() << "\n";
        throw std::runtime_error("File Stream is invalid");
    }
    return s;
}

/**
 *  Class for reading the Tif image file directory
 * 
 * */

class TiffIFD {

private:
    std::basic_istream<char>& _stream;
    uint_t _address;

    ushort_t    _NumDirEntries;    // Number of Tags in IFD
	uint_t   _NextIFDOffset;    // Offset to next IFD
    std::vector<TiffTag> _tagList; // List of Tags
 
    std::shared_ptr<VirtualEndianHandler> _endian_handler;
public:

    //delete the default constructor, such that the user always has a valid stream 
    TiffIFD() = delete;

    TiffIFD(std::basic_istream<char>& stream, uint_t address, std::shared_ptr<VirtualEndianHandler> endian_handler) : _stream(stream), _address(address), _endian_handler(endian_handler) {}

    TiffTag convert_to_tag(std::array<char, TiffTag_size> data) {
        TiffTag tag;
        std::array<char, 2>id{data.at(0), data.at(1)};
        tag.TagID = this->_endian_handler->convert(id);

        tag.DataType = this->_endian_handler->convert(std::array<char, 2>{data.at(2), data.at(3)});

        tag.DataCount = this->_endian_handler->convert(std::array<char, 4>{data.at(4), data.at(5),
                                                data.at(6), data.at(7)});
                                
        tag.DataOffset = this->_endian_handler->convert(std::array<char, 4>{data.at(8), data.at(9),
                                                data.at(10), data.at(11)});
        return tag;
    }

    void printTag(const TiffTag tag) {
#if PRINT == 1 
        std::cout << "TagID " << std::format("0x{:02X} ", tag.TagID) << to_string(TiffTagType(tag.TagID))
                << " DataType " << std::format("0x{:02X} ", tag.DataType) << to_string(TiffDataType(tag.DataType))
                << " DataCount " << std::format("0x{:04X}", tag.DataCount)
                << " DataOffset " << std::format("0x{:04X}", tag.DataOffset)
                << "\n";
#endif
    }

    void printTagRead(const TiffTagRead tag) {
#if PRINT == 1 
        std::cout << "---------------------------------------------\n";
        std::cout << "TagID: " << static_cast<int>(tag.TagID) << " " << to_string(tag.TagID) << "\n";

        for(auto i : tag.data) {
            std::visit([] (auto el){
                std::cout << to_string(el);
            }, i);
            std::cout << "\n";
        }
#endif
    }

    std::basic_istream<char>& get_stream() {
        return this->_stream;
    }

    TiffTag getTag(const TiffTagType tag_type) const {
        const auto ret = std::find_if(this->_tagList.begin(), this->_tagList.end(),
            [&tag_type](const TiffTag& tag) -> bool {
                return (TiffTagType( tag.TagID) == tag_type);
            });

        if (ret == this->_tagList.end()) {
            throw std::range_error(std::format("TiffTagType was not found {}", static_cast<int>(tag_type)));
        }

        return *ret;
    }

    TiffTagRead read_tiff_tag(const TiffTag& tag) const {
        const uint_t offset = tag.DataOffset;

        const TiffDataType data_type = TiffDataType(tag.DataType);
        const uint_t data_count = tag.DataCount;

        TiffTagRead tag_read{TiffTagType(tag.TagID), std::vector<TiffDataVariant>() };

        if(get_variant_element_size(make_variant(data_type)) * size_t(data_count) <= 4) {
            for(uint_t i = 0; i < data_count; i++) {
                TiffDataVariant variant = make_variant(data_type);
                    std::visit([&,this](auto& el) {
                    using T = std::decay_t<decltype(el)>;
                    
                    if constexpr( std::is_same_v<T, std::vector<ushort_t>>) {
                        if(i > 1) {
                            throw std::runtime_error("Error: i must <= 1");
                        }
                        el.push_back( this->_endian_handler->convert(offset, i) );
                    } else if constexpr ( std::is_same_v<T, std::vector<uint_t>>) {
                        if(i > 0) {
                            throw std::runtime_error("Error: i must be 0");
                        }
                        el.push_back( offset );
                    } else if constexpr ( std::is_same_v<T, std::vector<uint64_t>>) {
                        throw std::logic_error("uint64 not valid");
                    } else if constexpr ( std::is_same_v<T, std::vector<uint8_t>>) {
                        throw std::logic_error("uint8_t not valid");
                    } else if constexpr ( std::is_same_v<T, std::string>) {
                        throw std::logic_error("string not valid");
                    } else {
                        static_assert(0, "Error unhandled variant type");
                    }
        
                }, variant);
                tag_read.data.push_back(variant);
            }
        } else {        
            this->_stream.seekg(offset);

            for(uint_t i = 0; i < data_count; i++) {
                TiffDataVariant variant = make_variant(data_type);
                std::visit([&, this](auto& el) {
                    using T = std::decay_t<decltype(el)>;

                    if constexpr( std::is_same_v<T, std::vector<uint8_t>>) {
                        std::array<char, 1> arr; 
                        read_char<1>(arr, this->_stream);
                        el.push_back( this->_endian_handler->convert(arr) );
                    } else if constexpr( std::is_same_v<T, std::vector<ushort_t>>) {
                        std::array<char, 2> arr; 
                        read_char<2>(arr, this->_stream);
                        el.push_back( this->_endian_handler->convert(arr) );
                    } else if constexpr ( std::is_same_v<T, std::vector<uint_t>>) {
                        std::array<char, 4> arr; 
                        read_char<4>(arr, this->_stream);
                        el.push_back(  this->_endian_handler->convert(arr) );
                    } else if constexpr ( std::is_same_v<T, std::vector<uint64_t>>) {
                        std::array<char, 8> arr; 
                        read_char<8>(arr, this->_stream);
                        el.push_back( this->_endian_handler->convert(arr) );
                    } else if constexpr ( std::is_same_v<T, std::string>) {
                        el = read_char_str(this->_stream, data_count);
                    } else {
                        static_assert(0, "Error unhandled variant type");
                    }
                }, variant);
                tag_read.data.push_back(variant);

                //stop loop when variant type is std::string
                //then all data are stored in the string
                if(std::holds_alternative<std::string>(tag_read.data.back()))
                    break;
            }
        }
        return tag_read;
    }

    void read() {
        this->_stream.seekg(this->_address);

        std::array<char, 2> numEntries_char;
        read_char<2>(numEntries_char, this->_stream);
        this->_NumDirEntries = this->_endian_handler->convert(numEntries_char);
        
        auto pos0 = this->_stream.tellg();

        std::array<char, TiffTag_size> tag_char;
        
        for(uint_t i = 0; i < this->_NumDirEntries; i++) {
            std::streampos pos = this->_stream.tellg();
            read_char<TiffTag_size>(tag_char, this->_stream);

            auto tag = this->convert_to_tag(tag_char);
            this->_tagList.push_back(
                tag
            ); 
            this->printTag(tag);
        }

        std::array<char, 2> next_idf_offset_char;
        read_char<2>(next_idf_offset_char, this->_stream);
        this->_NextIFDOffset = this->_endian_handler->convert(next_idf_offset_char);

#if PRINT == 1
        std::cout << "_NextIFDOffset " << this->_NextIFDOffset << "\n";
        std::cout << "stream good " << this->_stream.good() << "\n";
#endif

        for(auto el : this->_tagList) {
            TiffTagRead tag0 = this->read_tiff_tag(el);
            this->printTagRead(tag0);
        }
    };

    std::shared_ptr<VirtualEndianHandler> get_endian_handler() {
        return this->_endian_handler;
    }


};


/**
 * Class to read the image data
 * IFDs must be read before creating this class
 */
class TiffReadStrips {
    std::basic_istream<char>& _stream;
    const TiffIFD& _ifd;
    const std::shared_ptr<VirtualEndianHandler> _endian_handler;
public:

    // delete the default constructor. 
    TiffReadStrips() = delete;

    explicit TiffReadStrips(TiffIFD& ifd, std::shared_ptr<VirtualEndianHandler> endian_handler) : _ifd(ifd), _stream(ifd.get_stream()), _endian_handler(endian_handler) {}

    virtual void reformat_strip(std::string& strip) {}

    /**
     * Read the metadata
     */
    TiffTagRead get_metadata_tag(TiffTagType tag, const size_t size) const {
        if(!this->_stream) {
            throw std::runtime_error("Error: file stream not valid");
        }

        TiffTag width = this->_ifd.getTag(tag);

        TiffTagRead res = this->_ifd.read_tiff_tag(width);
        
        if(res.data.size() != size) {
            throw std::runtime_error(std::format("Error tag data size is {} expected {}\n", res.data.size(), size));
        }

        return res;
    }

    /**
     * Read the image data
     */
    std::string readStrips() {
        if(!this->_stream) {
            throw std::runtime_error("Error: file stream not valid");
        }
        //TiffTag getTag
        //ImageX
        const TiffTagRead image_width = this->get_metadata_tag(TiffTagType::ImageWidth, 1);
        const TiffTagRead image_length = this->get_metadata_tag(TiffTagType::ImageLength, 1);
        const TiffTagRead strip_byte_count = this->get_metadata_tag(TiffTagType::StripByteCounts, 1);
        const TiffTagRead strip_offset = this->get_metadata_tag(TiffTagType::StripOffsets, 1);
        const TiffTagRead rows_per_strip = this->get_metadata_tag(TiffTagType::RowsPerStrip, 1);
        const TiffTagRead samples_per_pixel = this->get_metadata_tag(TiffTagType::SamplesPerPixel, 1);

        std::cout << "samples_per_pixel " << to_ulong(samples_per_pixel.data[0]) << "\n";

        TiffDataVariant one = make_variant(uint_t{1});

        if( samples_per_pixel.data[0] != one) {
            const TiffTagRead planar_config = this->get_metadata_tag(TiffTagType::PlanarConfiguration, 1);
            std::cout << "planar_config " << to_ulong(planar_config.data[0]) << "\n";
        }

        TiffDataVariant strips_in_image_var = (image_length.data[0] + rows_per_strip.data[0] - one ) / rows_per_strip.data[0];
        uint64_t strips_in_image = to_ulong( strips_in_image_var );

        std::cout << "Data length " << to_string(image_length.data[0] + rows_per_strip.data[0] - one )  << "\n";
        std::cout << "strips_in_image " << strips_in_image << "\n";

        std::string image_data;
        for(uint64_t i = 0; i < strips_in_image; ++i) {

            if(i < strip_offset.data.size()) {
                this->_stream.seekg(static_cast<std::streamoff>(to_size_t(strip_offset.data.at(i))));
                size_t bytes_read = static_cast<size_t>(to_ulong(strip_byte_count.data[0]));

                std::string strip = read_char_str(this->_stream, bytes_read);
                reformat_strip(strip);
                image_data.append(strip);
            }
        }
        return image_data;
    }

    /**
     * Return an rgb image
     */
    ImageContainer<uint8_t> get_image() {
        const TiffTagRead image_width = this->get_metadata_tag(TiffTagType::ImageWidth, 1);
        const TiffTagRead image_ppc = this->get_metadata_tag(TiffTagType::SamplesPerPixel, 1);
        ImageContainer<uint8_t> result(to_size_t(image_width.data[0]), 3);


        std::string strips = this->readStrips();
        result.append_row<std::string>(strips);

        return result;
    }

};



class TiffReadStrips_RGB : public TiffReadStrips {

    TiffReadStrips_RGB(TiffIFD& ifd, std::shared_ptr<VirtualEndianHandler> endian_handler) : TiffReadStrips(ifd, endian_handler) {}
};

};