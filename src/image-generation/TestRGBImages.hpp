#include "TestAbstractImage.hpp"

class TestImageSingleColor : public TestAbstractImage<uint8_t> {
    const uint8_t _color;
    const size_t _rows;
    const size_t _columns;
    const size_t _pixel;

    public:
        explicit TestImageSingleColor(const size_t n, const size_t m, size_t pixel, const uint8_t color);
        void generateImage();
    
};

/**
 * Configuration for the Ramp
 * HORIZAONTAL: color increases with increasing column index
 * VERTICAL: color increases with increasing row index
 */
enum class RampType{HORIZONTAL, VERTICAL};

class TestImageRampRGB : public TestAbstractImage<uint8_t> {
    //member variables
    const uint8_t _color_min;
    const uint8_t _color_max;
    const size_t _rows;
    const size_t _columns;

    const RampType _type;
    const std::array<size_t, 3> _idx;

    public:
        explicit TestImageRampRGB(const size_t n, const size_t m, RampType type, const uint8_t color_min, 
            const uint8_t color_max, const std::array<size_t, 3> idx);
        void generateImage();
   
    private:

    /**
     * helper Function to compute the ramp's RGB values based on the given type (this->_type)
     * @Args:
     *      i: row index
     *      j: column index
     */
    inline std::array<uint8_t, 3> calc_function(const size_t i, const size_t j);

};


