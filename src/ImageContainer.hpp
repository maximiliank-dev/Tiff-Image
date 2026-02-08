#pragma once

#include <array>
#include <format>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

template <typename T>
concept Iterable = std::ranges::range<T>;  // begin/end valid

template <typename T>
concept Sizeable = requires(T a) {
    { a.size() } -> std::same_as<std::size_t>;
};

template <typename T>
concept IterableSize = Iterable<T> && Sizeable<T>;

/**
 * Image Container
 * Data is stored in row major order
 */
template <typename PT>
class ImageContainer {
   private:
    std::vector<PT> _img;

    size_t _row_length;
    size_t _pixel_number_of_colors;

   public:
    ImageContainer(const size_t row_length, const size_t pixel_number_of_colors)
        : _row_length(row_length),
          _pixel_number_of_colors(pixel_number_of_colors) {}
    ImageContainer(ImageContainer&& other)
        : _row_length(other._row_length),
          _pixel_number_of_colors(other._pixel_number_of_colors),
          _img(std::move(other._img)) {}

    ~ImageContainer() = default;

    size_t get_column_length() const noexcept {
        return static_cast<uint64_t>(this->_row_length);
    }

    size_t get_height() const noexcept {
        return this->_img.size() /
               (this->_row_length * this->_pixel_number_of_colors);
    }

    size_t get_width() const noexcept {
        return static_cast<uint64_t>(this->_row_length);
    }

    size_t get_total_size() const noexcept { return this->_img.size(); }

    size_t get_pixel_number_of_colors() const noexcept {
        return this->_pixel_number_of_colors;
    }

    size_t get_bytes_per_row() const noexcept {
        return this->_row_length * this->_pixel_number_of_colors - 1;
    }

    /**
     * Move assignment
     */
    ImageContainer& operator=(ImageContainer&& other) {
        this->_row_length = other._row_length;
        this->_pixel_number_of_colors = other._pixel_number_of_colors;
        this->_img = std::move(other._img);
        return *this;
    }

    template <IterableSize T>
    void append_row(T container) {
        if (container.size() %
                (this->_row_length * this->_pixel_number_of_colors) !=
            0) {
            throw std::runtime_error(
                std::format("error container size is {} expected {}",
                            container.size(),
                            this->_row_length));
        }

        _img.reserve(container.size());
        for (auto it = container.begin(); it != container.end(); it++) {
            _img.push_back(*it);
        }
    }

    /**
     * Resize the image size
     * @param rows: number of rows of the new image
     */
    void resize(size_t rows) {
        this->img_.clear();

        const size_t size = rows*this->_row_length * this->_pixel_number_of_colors;
        this->_img.resize(size, 0);
    }

    void print() {
        std::cout << "Image Data\n";
        size_t elements_per_row =
            this->_row_length * this->_pixel_number_of_colors;
        size_t cnt = 0;
        for (auto it : this->_img) {
            std::cout << (*it).to_string() << " ";
            cnt++;
            if (cnt == elements_per_row) {
                std::cout << "\n";
                cnt = 0;
            }
        }
    }

    const std::vector<PT>& get_data() const { return this->_img; }

    /**
     * Easy access function for the image data
     */
    PT at(size_t row, size_t col, size_t px) const {
        if (this->get_height() <= row) {
            throw std::runtime_error(
                std::format("Error: invalid row id: got {} max {}",
                            row,
                            this->get_height()));
        }
        if (this->get_width() <= col) {
            throw std::runtime_error(
                std::format("Error: invalid col id: got {} max {}",
                            col,
                            this->get_width()));
        }
        if (this->_pixel_number_of_colors <= px) {
            throw std::runtime_error(
                std::format("Error: invalid pixel id: got {} max {}",
                            px,
                            this->_pixel_number_of_colors));
        }
        const size_t cols = this->get_width();
        const size_t pixels = this->_pixel_number_of_colors;

        return this->_img.at(row * (cols * pixels) + col * pixels + px);
    }

    /**
     * create a new image with transposed image data
     */
    ImageContainer transpose() {
        ImageContainer img_t(this->_row_length, this->_pixel_number_of_colors);
        img_t.resize(this->get_height());

        for(size_t i = 0; i < this->get_height(); i++) {
            for(size_t j = 0; j < this->get_width(); j++) {
                for(size_t k = 0; k < this->_pixel_number_of_colors; k++) {
                    img_t.at(j,i,k) = this->at(i,j,k);
                }
            }
        }
        return img_t;
    }
};