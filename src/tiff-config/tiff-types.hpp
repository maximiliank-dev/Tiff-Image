#pragma once

#include <string>
#include <format>

#include "types.hpp"


struct TiffTag {
    ushort_t TagID;
    ushort_t DataType;
    uint_t DataCount;
    uint_t DataOffset;
};
constexpr unsigned int TiffTag_size = sizeof(TiffTag);

enum class TiffDataType { BYTE=1, ASCII=2, SHORT=3, LONG=4, RATIONAL=5 };

enum class TiffTagType { NewSubfileType=254, ImageWidth=256, ImageLength=257, BitsPerSample=258, Compression=259, 
    PhotometricInterpretation=262, StripOffsets=273, Orientation=274,
    SamplesPerPixel=277, RowsPerStrip=278, StripByteCounts=279, 
    XResolution=282, YResolution=283, PlanarConfiguration=284, PageName=285,
    ResolutionUnit=296, PageNumber=287, TileWidth=322, TileLength=323, TileOffsets=324, TileByteCounts=325, SampleFormat=339  };


// define the type for each Tiff Tag Field
template<TiffTagType t>
struct TagTypeValue;

template<> struct TagTypeValue<TiffTagType::ImageWidth> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::ImageLength> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::BitsPerSample> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::Compression> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::PhotometricInterpretation> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::StripOffsets> { using type = ushort_t; };
template<> struct TagTypeValue<TiffTagType::XResolution> { using type = uint64_t; };
template<> struct TagTypeValue<TiffTagType::YResolution> { using type = uint64_t; };

inline TiffDataType get_tag_data_type(TiffTagType tag) {
    switch(tag) {
        case TiffTagType::NewSubfileType:
            return TiffDataType::LONG;
        case TiffTagType::PhotometricInterpretation:
            return TiffDataType::SHORT;
        case TiffTagType::Compression:
            return TiffDataType::SHORT;
        case TiffTagType::ImageWidth:
            return TiffDataType::SHORT;
        case TiffTagType::ImageLength:
            return TiffDataType::SHORT;
        case TiffTagType::ResolutionUnit:
            return TiffDataType::SHORT;
        case TiffTagType::BitsPerSample:
            return TiffDataType::SHORT;
        case TiffTagType::XResolution:
            return TiffDataType::RATIONAL;
        case TiffTagType::YResolution:
            return TiffDataType::RATIONAL;
        case TiffTagType::RowsPerStrip:
            return TiffDataType::SHORT;
        case TiffTagType::StripOffsets:
            return TiffDataType::SHORT;
        case TiffTagType::StripByteCounts:
            return TiffDataType::SHORT;
        default:
            throw std::runtime_error("Error type not specified");
    }
}

inline std::string to_string(TiffDataType type) {
    switch(type) {
        case TiffDataType::BYTE: return "BYTE";
        case TiffDataType::ASCII: return "ASCII";
        case TiffDataType::SHORT: return "SHORT";
        case TiffDataType::LONG: return "LONG";
        case TiffDataType::RATIONAL: return "RATIONAL";
    }
    return "unknown";
}

inline std::string to_string(TiffTagType type) {
    switch(type) {
        case TiffTagType::NewSubfileType: return "NewSubfileType";
        case TiffTagType::ImageWidth: return "ImageWidth";
        case TiffTagType::ImageLength: return "ImageLength";
        case TiffTagType::BitsPerSample: return "BitsPerSample";
        case TiffTagType::Compression: return "Compression";
        case TiffTagType::PhotometricInterpretation: return "PhotometricInterpretation";
        case TiffTagType::StripOffsets: return "StripOffsets";
        case TiffTagType::Orientation: return "Orientation";
        case TiffTagType::SamplesPerPixel: return "SamplesPerPixel";
        case TiffTagType::RowsPerStrip: return "RowsPerStrip";
        case TiffTagType::StripByteCounts: return "StripByteCounts";
        case TiffTagType::XResolution: return "XResolution";
        case TiffTagType::YResolution: return "YResolution";
        case TiffTagType::PlanarConfiguration: return "PlanarConfiguration";
        case TiffTagType::ResolutionUnit: return "ResolutionUnit";
        case TiffTagType::PageNumber: return "PageNumber";
        case TiffTagType::TileWidth: return "TileWidth";
        case TiffTagType::TileLength: return "TileLength";
        case TiffTagType::TileOffsets: return "TileOffsets";
        case TiffTagType::TileByteCounts: return "TileByteCounts";
        case TiffTagType::PageName: return "PageName";
        case TiffTagType::SampleFormat: return "SampleFormat";
    }
    return std::format("unkwon - {}", 0);
}

enum class tiff_header_endian : uint16_t { LITTLE=0x4949, BIG=0x4D4D }; 
