#pragma once

#include <vector>
#include <iterator>
#include <format>
#include <string>
#include <iostream>
#include <array>

template<typename T>
concept Iterable = std::ranges::range<T>;  // begin/end valid


template<typename T>
concept Sizeable = requires(T a) {
    { a.size() } -> std::same_as<std::size_t>;
};

template<typename T>
concept IterableSize = Iterable<T> && Sizeable<T>;

/*
template<typename T>
concept Pixel = requires (T a, T b) {
    a+b;
};

struct RGB {
    using element_type = uint8_t;
    std::array<uint8_t, 3> values{};

    uint8_t& r() { return this->values.at(0);};
    uint8_t& g() { return this->values.at(1);};
    uint8_t& b() { return this->values.at(2);};

    static constexpr std::size_t size() { return 3; }

    uint8_t& operator[](std::size_t i) {
        return values.at(i);
    }


};
std::string to_string(RGB& px);
RGB operator+(const RGB& a, const RGB& b);
*/

template<typename PT>
class ImageContainer {

private:
    std::vector<std::vector<PT>> _img;

    const size_t _col_size; 
    const size_t _pixel_number_of_colors;

public:

    ImageContainer(const size_t col_size, const size_t pixel_number_of_colors) : _col_size(col_size), _pixel_number_of_colors(pixel_number_of_colors) {}

    ~ImageContainer() = default;


    uint64_t get_width() noexcept {
        return this->_col_size;
    }

    uint64_t get_hight() noexcept {
        return static_cast<uint64_t>(_img->size())/this->_col_size;
    }

    uint64_t get_total_size() noexcept {
        return this->_col_size*this->_img.size();
    }


    template<IterableSize T>
    void append_columns(T container) {
        if(container.size() % (this->_col_size*this->_pixel_number_of_colors) != 0) {
            throw std::runtime_error(std::format("error container size is {} expected {} \n", container.size(), this->_col_size));
        }

        _img.reserve(this->_col_size);
        for(auto it = container.begin(); it != container.end(); it++) {
            _img.push_back(std::vector<PT>(this->_pixel_number_of_colors));

            std::vector<PT>& px = _img.back();
            //split the list into the datatype 
            for(std::size_t i = 0; i < this->_pixel_number_of_colors; ++i) {
                //error detection if there are too few elements in the container
                if(it == container.end()) {
                    throw std::runtime_error(std::format("Error: Data is not subdividable into blocks of {} elements", 
                        this->_pixel_number_of_colors));
                }
            
                px[i] = (*it);
                it++;
            }
        }
    }

    void print() {
        std::cout << "Image Data\n";
        for(auto it : this->_img) {
            std::cout << (*it).to_string() << " ";
        }
        std::cout << "\n";
    }

    const std::vector<std::vector<PT>> get_data() const {
        return this->_img;
    }


};