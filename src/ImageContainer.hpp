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


template<typename PT>
class ImageContainer {

private:
    std::vector<PT> _img;

    const size_t _row_length; 
    const size_t _pixel_number_of_colors;

public:

    ImageContainer(const size_t col_size, const size_t pixel_number_of_colors) : _row_length(col_size), _pixel_number_of_colors(pixel_number_of_colors) {}

    ~ImageContainer() = default;


    size_t get_height() const noexcept {
        return this->_img.size()/(this->_row_length * this->_pixel_number_of_colors);
    }

    size_t get_width() const noexcept {
        return static_cast<uint64_t>(this->_row_length);
    }

    size_t get_total_size() const noexcept {
        return this->_img.size();
    }

    size_t get_pixel_number_of_colors() const noexcept {
        return this->_pixel_number_of_colors;
    }

    size_t get_bytes_per_row() const noexcept {
        return this->_row_length*this->_pixel_number_of_colors-1;
    }

    template<IterableSize T>
    void append_row(T container) {
        if(container.size() % (this->_row_length*this->_pixel_number_of_colors) != 0) {
            throw std::runtime_error(std::format("error container size is {} expected {} \n", container.size(), this->_row_length));
        }

        _img.reserve(container.size()*(this->_pixel_number_of_colors));
        for(auto it = container.begin(); it != container.end(); it++) {
            _img.push_back(*it);
        }
    }

    void print() {
        std::cout << "Image Data\n";
        for(auto it : this->_img) {
            std::cout << (*it).to_string() << " ";
        }
        std::cout << "\n";
    }

    const std::vector<PT>& get_data() const {
        return this->_img;
    }

    /**
     * Easy access function for the image data
     */
    PT at(size_t row, size_t col, size_t px) const {

        if( this->get_height() <= row) {
            throw std::runtime_error(std::format("Error: invalid row id: got {} max {}", row, this->get_height()));
        }
        if( this->get_width() <= col) {
            throw std::runtime_error(std::format("Error: invalid col id: got {} max {}", col, this->get_width()));
        }
        if( this->_pixel_number_of_colors <= px) {
            throw std::runtime_error(std::format("Error: invalid pixel id: got {} max {}", px, this->_pixel_number_of_colors));
        }
        const size_t cols = this->get_width();
        const size_t pixels = this->_pixel_number_of_colors;

        return this->_img.at(row*(cols*pixels) + col*pixels + px );
    }

};