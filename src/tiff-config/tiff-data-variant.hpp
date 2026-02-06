#pragma once

#include <variant>

#include "types.hpp"
#include "tiff-types.hpp"


using TiffDataVariant = std::variant<uint8_t, ushort_t, uint_t, uint64_t, std::string>;


template<typename T>
inline T safe_convert(uint64_t value) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    static_assert(!std::is_same_v<T, bool>, "T must not be bool type");

    if( value > std::numeric_limits<T>::max()) {
        throw std::runtime_error("Error conversion failed");
    }
    return T(value);
}

/**
 * Map the Tiff data type to the TiffDataVariant entries
 */
inline TiffDataVariant make_variant(const TiffDataType type) {
    switch(type) {
        case TiffDataType::BYTE: return uint8_t{};
        case TiffDataType::ASCII: return std::string{"\t"};
        case TiffDataType::SHORT: return ushort_t{};
        case TiffDataType::LONG: return uint_t{};
        case TiffDataType::RATIONAL: return uint64_t{};
    }
    throw std::runtime_error("Error type is not supported");
}

/**
 * Map the Tiff data type to the TiffDataVariant entries
 */
template<typename T>
inline TiffDataVariant make_variant(const TiffDataType type, T value) {
    switch(type) {
        case TiffDataType::BYTE: return safe_convert<uint8_t>(value);
        case TiffDataType::ASCII: return std::to_string(value);
        case TiffDataType::SHORT: return safe_convert<ushort_t>(value);
        case TiffDataType::LONG: return safe_convert<uint_t>(value);
        case TiffDataType::RATIONAL: return safe_convert<uint64_t>(value);
    }
    throw std::runtime_error("Error type is not supported");
}

inline TiffDataVariant make_variant(const TiffTagType tag, uint64_t value ) {
    auto type = get_tag_data_type(tag);
    return make_variant(type, value);
}

inline std::size_t get_variant_element_size(const TiffDataVariant variant) {
    return std::visit( [](auto& el ){
        return sizeof(el);
    }, variant);
}

inline TiffDataVariant operator+(const TiffDataVariant& a, const TiffDataVariant& b)
{
    return std::visit([](auto& x, auto& y) -> TiffDataVariant {
        using A = std::decay_t<decltype(x)>;
        using B = std::decay_t<decltype(y)>;
        
        if constexpr (std::is_integral<A>::value && std::is_integral<B>::value ) { // && std::is_integral<B> 
            return static_cast<uint64_t>(x) + static_cast<uint64_t>(y);
        } else {
            throw std::runtime_error("Error operator + : variant is not of integral type");
        }
    }, a, b);
}

inline TiffDataVariant operator-(const TiffDataVariant& a, const TiffDataVariant& b)
{
    return std::visit([](auto& a, auto& b) -> TiffDataVariant {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;
        
        if constexpr (std::is_integral<A>::value && std::is_integral<B>::value ) {
            return static_cast<uint64_t>(a) - static_cast<uint64_t>(b); 
        } else {
            throw std::runtime_error("Error operator - : variant is not of integral type");
        }
    }, a, b);
}

inline TiffDataVariant operator*(const TiffDataVariant& a, const TiffDataVariant& b)
{
    return std::visit([](auto& a, auto& b) -> TiffDataVariant {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;
        
        if constexpr ( std::is_integral<A>::value && std::is_integral<B>::value ) {
            return static_cast<uint64_t>(a) * static_cast<uint64_t>(b); 
        } else {
            throw std::runtime_error("Error operator * : variant is not of integral type");
        }
    }, a, b);
}


inline TiffDataVariant operator/(const TiffDataVariant& a, const TiffDataVariant& b)
{
    return std::visit([](auto& a, auto& b) -> TiffDataVariant {
        using A = std::decay_t<decltype(a)>;
        using B = std::decay_t<decltype(b)>;
        
        if constexpr ( std::is_integral<A>::value && std::is_integral<B>::value ) {
            return static_cast<uint64_t>(a) / static_cast<uint64_t>(b); 
        } else {
            throw std::runtime_error("Error operator / : variant is not of integral type");
        }
    }, a, b);
}

inline uint64_t to_ulong(const TiffDataVariant& a) {

    return std::visit( [](auto& el) -> uint64_t {
        using A = std::decay_t<decltype(el)>;
        if constexpr ( std::is_integral<A>::value) {
            return static_cast<uint64_t>(el);
        }
        std::string k =  std::format("Conversion Error: variant is not of integral type got {}", typeid(A).name());
        throw std::domain_error(std::format("Conversion Error: variant is not of integral type got {}", typeid(A).name()));
    }, a);
}

/**
 * Convert the variant to a size_t type
 */
inline size_t to_size_t(const TiffDataVariant& a) {
    uint64_t v = to_ulong(a);
    if( v > static_cast<uint64_t>(std::numeric_limits<size_t>::max()) ) {
        throw std::out_of_range("Value too large for size_t");
    }
    return static_cast<size_t>(v);
}

// /**
//  * Cast the variant to an array of size_n
//  * @throws cast error, if the data type does not fit into N chars
//  */
// inline std::array<char, 1> make_array(const TiffDataVariant& variant, VirtualEndianHandler& handler) {
//     return std::visit([&handler](auto el)  -> std::array<char, 1> {
//          using T = std::decay_t<decltype(el)>; 
//          if constexpr( std::is_same<T, uint8_t>::value) {
//             uint8_t& el_loc = el;
//             return handler.convert_to_array(el_loc);

//          } else if constexpr ( std::is_same<T, std::string>::value) { // return the string value
//             if(el.size() == 1) {
//                 return std::array<char, 1>{el.at(0)};
//             } else {
//                 throw std::runtime_error("Error string is too long, max length must be 1");
//             }
//         }
//         throw std::runtime_error("Error only uint8 or string can be casted");
//     }, variant);
// }