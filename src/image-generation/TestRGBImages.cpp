#include <vector>
#include "TestRGBImages.hpp"

TestImageSingleColor::TestImageSingleColor(size_t n, size_t m, size_t pixel, uint8_t color) : 
    _color(color), _pixel(pixel), _rows(n), _columns(m), TestAbstractImage<uint8_t>(m, 3) {
}

void TestImageSingleColor::generateImage() {
    size_t n = this->img->get_column_length()*this->img->get_pixel_number_of_colors();
    std::vector<uint8_t> v(n, uint8_t{0});
    for(size_t i = 0; i < this->_rows; i++ ) {
        v = std::vector<uint8_t>(n, uint8_t{0});
        for(size_t j = 0; j < n; j += this->img->get_pixel_number_of_colors() ) {
            std::array<uint8_t, 3> dt {0x0, 0x0, 0x0};
            for(size_t k = 0; k < 3; ++k ) {
                v[j+k] = dt[k];
            }
            v[j+this->_pixel] = this->_color;
        }
        this->img->append_row(v);
    }
}
TestImageRampRGB::TestImageRampRGB(const size_t n, const size_t m, RampType type, const uint8_t color_min, 
            const uint8_t color_max, const std::array<size_t, 3> idx) : 
    _color_min(color_min), _color_max(color_max), _idx(idx), _rows(n), _columns(m), _type(type), TestAbstractImage<uint8_t>(m, 3) {
}

inline std::array<uint8_t, 3> TestImageRampRGB::calc_function(const size_t i, const size_t j) {

    std::array<uint8_t, 3> res{0,0,0};
    uint8_t max_width;
    
    switch (this->_type) {
        case RampType::HORIZONTAL:
            max_width = static_cast<uint8_t>( std::min( static_cast<size_t>(std::numeric_limits<uint8_t>::max()), this->_columns ) ); 
            break;
        case RampType::VERTICAL:
            max_width = static_cast<uint8_t>( std::min( static_cast<size_t>(std::numeric_limits<uint8_t>::max()), this->_rows ) ); 
            break;
    }

    switch(this->_type) {
        case RampType::HORIZONTAL:
            for(size_t it = 0; it < 3; it++) {
                if(this->_idx[it])
                    res[it] = this->_color_min + static_cast<uint8_t>(j);
            }
            break;
        case RampType::VERTICAL:
            for(size_t it = 0; it < 3; it++) {
                if(this->_idx[it])
                    res[it] = this->_color_min + static_cast<uint8_t>(i);
            }
            break;
    }

    return res;
}

void TestImageRampRGB::generateImage() {
    size_t n = this->img->get_column_length()*this->img->get_pixel_number_of_colors();
    std::vector<uint8_t> v(n, uint8_t{0});
    for(size_t i = 0; i < this->_rows; i++ ) {
        v = std::vector<uint8_t>(n, uint8_t{0});
        for(size_t j = 0; j < n; j += this->img->get_pixel_number_of_colors() ) {
            std::array<uint8_t, 3> dt = this->calc_function(i, j);
            for(size_t k = 0; k < 3; ++k ) {
                v[j+k] = dt[k];
            }
        }
        this->img->append_row(v);
    }
}

