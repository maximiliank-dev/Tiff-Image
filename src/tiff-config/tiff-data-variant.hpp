#pragma once

#include <algorithm>
#include <execution>
#include <functional>
#include <variant>
#include <vector>

#include "tiff-types.hpp"
#include "types.hpp"

using TiffDataVariant =
    std::variant<std::vector<uint8_t>, std::vector<ushort_t>,
                 std::vector<uint_t>, std::vector<uint64_t>, std::string>;

// helper functions
// value is true when vector contains an integral type parameter
template <typename T>
struct is_integral_vector : std::false_type {};

template <typename T>
struct is_integral_vector<std::vector<T>>
    : std::bool_constant<std::is_integral_v<T>> {};

// extract the vector template type
template <typename T>
struct vector_element;

template <typename T>
struct vector_element<std::vector<T>> {
    using type = T;
};

template <typename T>
inline T safe_convert(uint64_t value) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    static_assert(!std::is_same_v<T, bool>, "T must not be bool type");

    if (value > std::numeric_limits<T>::max()) {
        throw std::runtime_error("Error conversion failed");
    }
    return T(value);
}

/**
 * Map the Tiff data type to the TiffDataVariant entries
 */
inline TiffDataVariant make_variant(const TiffDataType type) {
    switch (type) {
        case TiffDataType::BYTE:
            return std::vector<uint8_t>();
        case TiffDataType::ASCII:
            return "";
        case TiffDataType::SHORT:
            return std::vector<ushort_t>();
        case TiffDataType::LONG:
            return std::vector<uint_t>();
        case TiffDataType::RATIONAL:
            return std::vector<uint64_t>();
        default:
            throw std::runtime_error("Error type is not supported");
    }
}

/**
 * Map the Tiff data type to the TiffDataVariant entries
 */
template <typename T>
inline TiffDataVariant make_variant(const T value) {
    if constexpr (std::is_same_v<T, uint8_t>) {
        return std::vector<uint8_t>{safe_convert<uint8_t>(value)};
    } else if constexpr (std::is_same_v<T, std::string>) {
        return std::to_string(value);
    } else if constexpr (std::is_same_v<T, ushort_t>) {
        return std::vector<ushort_t>{safe_convert<ushort_t>(value)};
    } else if constexpr (std::is_same_v<T, uint_t>) {
        return std::vector<uint_t>{safe_convert<uint_t>(value)};
    } else if constexpr (std::is_same_v<T, uint64_t>) {
        return std::vector<uint64_t>{safe_convert<uint64_t>(value)};
    } else {
        throw std::runtime_error("Error type is not supported");
    }
}

/**
 * Helper Function to make the variant's vector out of an initializer list
 */
template <typename T, typename V>
inline std::vector<V> make_variant_vector(std::initializer_list<T> values) {
    std::vector<V> vec;
    vec.reserve(values.size());
    for (const T& el : values) {
        vec.push_back(safe_convert<V>(el));
    }
    return vec;
}

/**
 * Map the Tiff data type to the TiffDataVariant entries
 */
inline TiffDataVariant make_variant(
    const TiffDataType type, const std::initializer_list<uint64_t> values) {
    switch (type) {
        case TiffDataType::BYTE:
            return make_variant_vector<uint64_t, uint8_t>(values);
        case TiffDataType::ASCII: {
            std::string s;
            s.reserve(values.size());
            for (uint64_t v : values) {
                s.push_back(static_cast<char>(v));
            }
            return s;
        }
        case TiffDataType::SHORT:
            return make_variant_vector<uint64_t, ushort_t>(values);
        case TiffDataType::LONG:
            return make_variant_vector<uint64_t, uint_t>(values);
        case TiffDataType::RATIONAL:
            return make_variant_vector<uint64_t, uint64_t>(values);
        default:
            throw std::runtime_error("Error type is not supported");
    }
}

/**
 * Converts any uint64_t to the type required by the TiffTag
 */
inline TiffDataVariant make_variant(const TiffTagType tag,
                                    std::initializer_list<uint64_t> value) {
    auto type = get_tag_data_type(tag);
    return make_variant(type, value);
}

/**
 * Converts any uint64_t to the type required by the TiffTag
 */
// template<typename T>
inline TiffDataVariant make_variant(const TiffTagType tag, uint64_t value) {
    return make_variant(tag, {value});
}

/**
 * Get the element size of the container
 */
inline std::size_t get_variant_element_size(const TiffDataVariant variant) {
    return std::visit(
        [](auto& el) {
            using T = std::decay_t<decltype(el)>;

            return sizeof(T::value_type);
        },
        variant);
}

template <template <typename> typename F>
inline TiffDataVariant calc_binary(const TiffDataVariant& a,
                                   const TiffDataVariant& b) {
    return std::visit(
        [](auto& x, auto& y) -> TiffDataVariant {
            using A = std::decay_t<decltype(x)>;
            using B = std::decay_t<decltype(y)>;

            if constexpr (is_integral_vector<A>::value &&
                          is_integral_vector<A>::value) {
                using Ta = A::value_type;
                using Tb = B::value_type;
                std::vector<uint64_t> result(x.size());

                std::transform(x.begin(),
                               x.end(),
                               y.begin(),
                               result.begin(),
                               [](const Ta& ela, const Tb& elb) {
                                   // call the operation struct of the STL
                                   return F<uint64_t>{}(
                                       static_cast<uint64_t>(ela),
                                       static_cast<uint64_t>(elb));
                               });
                return result;
            } else {
                throw std::runtime_error(
                    "Error operator F : variant is not of integral type");
            }
        },
        a,
        b);
}

inline TiffDataVariant operator+(const TiffDataVariant& a,
                                 const TiffDataVariant& b) {
    return std::visit(
        [](auto& x, auto& y) -> TiffDataVariant {
            using A = std::decay_t<decltype(x)>;
            using B = std::decay_t<decltype(y)>;

            if constexpr (is_integral_vector<A>::value &&
                          std::is_same_v<A, B>) {
                using Ta = vector_element<A>::type;
                using Tb = vector_element<B>::type;
                using T = std::common_type_t<Ta, Tb>;
                std::vector<uint64_t> result(x.size());

                std::transform(x.begin(),
                               x.end(),
                               y.begin(),
                               result.begin(),
                               [](const Ta& ela, const Tb& elb) {
                                   return static_cast<uint64_t>(ela) +
                                          static_cast<uint64_t>(elb);
                               });
                return result;
            } else {
                throw std::runtime_error(
                    "Error operator + : variant is not of integral type");
            }
        },
        a,
        b);
}

inline TiffDataVariant operator-(const TiffDataVariant& a,
                                 const TiffDataVariant& b) {
    return calc_binary<std::minus>(a, b);
}

inline TiffDataVariant operator*(const TiffDataVariant& a,
                                 const TiffDataVariant& b) {
    return calc_binary<std::multiplies>(a, b);
}

/**
 * Returns a bool
 */
inline bool operator==(const TiffDataVariant& a, const TiffDataVariant& b) {
    return std::visit(
        [](auto& x, auto& y) -> bool {
            using A = std::decay_t<decltype(x)>;
            using B = std::decay_t<decltype(y)>;

            if constexpr (is_integral_vector<A>::value &&
                          is_integral_vector<A>::value) {
                if (x.size() != y.size()) {
                    return false;
                }
                return std::equal(x.begin(),
                                  x.end(),
                                  y.begin(),
                                  y.end(),
                                  [](auto el1, auto el2) {
                                      return static_cast<uint64_t>(el1) ==
                                             static_cast<uint64_t>(el2);
                                  });
            } else {
                throw std::runtime_error(
                    "Error operator == : variant is not of integral type");
            }
        },
        a,
        b);
}

inline TiffDataVariant operator/(const TiffDataVariant& a,
                                 const TiffDataVariant& b) {
    return calc_binary<std::divides>(a, b);
}

inline uint64_t to_ulong(const TiffDataVariant& a) {
    return std::visit(
        [](auto& el) -> uint64_t {
            using A = std::decay_t<decltype(el)>;

            if (el.size() != 1) {
                throw std::runtime_error(
                    "Error in to_ulong: vector size must be 1");
            }

            if constexpr (is_integral_vector<A>::value) {
                return static_cast<uint64_t>(el.at(0));
            } else {
                throw std::domain_error(std::format(
                    "Conversion Error: variant is not of integral type got {}",
                    typeid(A).name()));
            }
        },
        a);
}

inline size_t to_size_t(const TiffDataVariant& a) {
    return std::visit(
        [](auto& el) -> size_t {
            using A = std::decay_t<decltype(el)>;

            if (el.size() != 1) {
                throw std::runtime_error(
                    "Error in to_ulong: vector size must be 1");
            }

            if constexpr (is_integral_vector<A>::value) {
                uint64_t value = static_cast<uint64_t>(el.at(0));
                if (value <= std::numeric_limits<size_t>::max()) {
                    return static_cast<size_t>(value);
                } else {
                    throw std::runtime_error(
                        "Error uint64_t is too large: size_t overflows");
                }
            } else {
                throw std::domain_error(std::format(
                    "Conversion Error: variant is not of integral type got {}",
                    typeid(A).name()));
            }
        },
        a);
}

inline std::string to_string(TiffDataVariant a) {
    return std::visit(
        [](auto& el) -> std::string {
            using A = std::decay_t<decltype(el)>;

            std::string s{""};
            if constexpr (is_integral_vector<A>::value) {
                using T = A::value_type;  // element type of a vector

                for (T& v : el) {
                    s += std::to_string(v);
                }
                return s;
            } else if constexpr (std::is_same_v<A, std::string>) {
                return el;
            } else {
                throw std::domain_error(std::format(
                    "Conversion Error: variant is not of std::vector with "
                    "integral or of std::string type got {}",
                    typeid(A).name()));
            }
        },
        a);
}

// /**
//  * Cast the variant to an array of size_n
//  * @throws cast error, if the data type does not fit into N chars
//  */
// inline std::array<char, 1> make_array(const TiffDataVariant& variant,
// VirtualEndianHandler& handler) {
//     return std::visit([&handler](auto el)  -> std::array<char, 1> {
//          using T = std::decay_t<decltype(el)>;
//          if constexpr( std::is_same<T, uint8_t>::value) {
//             uint8_t& el_loc = el;
//             return handler.convert_to_array(el_loc);

//          } else if constexpr ( std::is_same<T, std::string>::value) { //
//          return the string value
//             if(el.size() == 1) {
//                 return std::array<char, 1>{el.at(0)};
//             } else {
//                 throw std::runtime_error("Error string is too long, max
//                 length must be 1");
//             }
//         }
//         throw std::runtime_error("Error only uint8 or string can be casted");
//     }, variant);
// }