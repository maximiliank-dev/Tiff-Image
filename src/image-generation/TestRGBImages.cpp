#include "TestRGBImages.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

TestImageSingleColor::TestImageSingleColor(size_t n, size_t m, size_t pixel,
                                           uint8_t color)
    : _color(color),
      _pixel(pixel),
      _rows(n),
      _columns(m),
      TestAbstractImage<uint8_t>(m, TestImageSingleColor::pixel_size) {}

void TestImageSingleColor::generateImage() {
    size_t n = this->img->get_column_length() *
               this->img->get_pixel_number_of_colors();
    std::vector<uint8_t> v(n, uint8_t{0});
    for (size_t i = 0; i < this->_rows; i++) {
        v = std::vector<uint8_t>(n, uint8_t{0});
        for (size_t j = 0; j < n;
             j += this->img->get_pixel_number_of_colors()) {
            std::array<uint8_t, 3> dt{0x0, 0x0, 0x0};
            for (size_t k = 0; k < 3; ++k) {
                v[j + k] = dt[k];
            }
            v[j + this->_pixel] = this->_color;
        }
        this->img->append_row(v);
    }
}
TestImageRampRGB::TestImageRampRGB(const size_t n, const size_t m,
                                   RampType type, const uint8_t color_min,
                                   const uint8_t color_max,
                                   const std::array<size_t, 3> idx)
    : _color_min(color_min),
      _color_max(color_max),
      _idx(idx),
      _rows(n),
      _columns(m),
      _type(type),
      TestAbstractImage<uint8_t>(m, 3) {}

inline std::array<uint8_t, 3> TestImageRampRGB::calc_function(
    const size_t i, const size_t j, const double theta, const uint8_t max_i,
    const uint8_t max_j) {
    std::array<uint8_t, 3> res{0, 0, 0};

    /**
     *  max_{x, y \in D} x·cosθ + y·sinθ
     * the max is for a given θ at the points: [ (0,0), (0,y_max), (x_max, 0),
     * (x_max, y_max) ] since the function is linear and constant for a given θ
     */
    constexpr size_t array_size = 4;
    std::array<double, array_size> pi{0, 0, 0, 0};
    std::array<std::pair<double, double>, array_size> points{
        std::make_pair(0, 0),
        std::make_pair(0, static_cast<double>(max_j)),
        std::make_pair(static_cast<double>(max_i), 0),
        std::make_pair(static_cast<double>(max_i), static_cast<double>(max_j))};

    for (size_t i = 0; i < array_size; i++) {
        pi[i] = std::get<0>(points[i]) * std::cos(theta) +
                std::get<1>(points[i]) * std::sin(theta);
    }

    double pmin = *std::min_element(pi.begin(), pi.end());
    double pmax = *std::max_element(pi.begin(), pi.end());

    for (size_t it = 0; it < 3; it++) {
        if (this->_idx[it]) {
            double px = static_cast<double>(i) * std::cos(theta) +
                        static_cast<double>(j) * std::sin(theta);
            // fxy is between 0 and 1
            double fxy = (px - pmin) / (pmax - pmin);
            res[it] = static_cast<uint8_t>(std::floor(255.0 * fxy));
        }
    }

    return res;
}

void TestImageRampRGB::generateImage() {
    size_t n = this->img->get_column_length() *
               this->img->get_pixel_number_of_colors();
    std::vector<uint8_t> v(n, uint8_t{0});

    double theta = 0.0;
    switch (this->_type) {
        case RampType::HORIZONTAL:
            theta = 0.0;
            break;
        case RampType::VERTICAL:
            // vertical is 90 degree
            theta = 3.1415926 / 2.0;
            break;
        case RampType::DIAGONAL:
            // diagonal is 45 degree
            theta = 3.1415926 / 4.0;
            break;
    }

    // convert image dimensions to uint8_t
    const uint8_t rows = (this->_rows <= std::numeric_limits<uint8_t>::max())
                             ? static_cast<uint8_t>(this->_rows)
                             : std::numeric_limits<uint8_t>::max();
    const uint8_t cols = (this->_columns <= std::numeric_limits<uint8_t>::max())
                             ? static_cast<uint8_t>(this->_columns)
                             : std::numeric_limits<uint8_t>::max();

    for (size_t i = 0; i < this->_rows; i++) {
        v = std::vector<uint8_t>(n, uint8_t{0});
        for (size_t j = 0; j < n;
             j += this->img->get_pixel_number_of_colors()) {
            // j/3 is because each pixel has 3 values
            std::array<uint8_t, 3> dt =
                this->calc_function(i,
                                    j / TestImageRampRGB::pixel_size,
                                    theta,
                                    rows,
                                    cols);
            for (size_t k = 0; k < 3; ++k) {
                v[j + k] = dt[k];
            }
        }
        this->img->append_row(v);
    }
}
