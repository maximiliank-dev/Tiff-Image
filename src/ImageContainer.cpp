#include "ImageContainer.hpp"

std::string to_string( RGB& px) {
    return std::format("({} {} {})", px.r(), px.g(), px.g());
}

RGB operator+(const RGB& a, const RGB& b) {
    RGB c;
    c.values.at(0) = a.values.at(0) + b.values.at(0);
    c.values.at(1) = a.values.at(1) + b.values.at(1);
    c.values.at(2) = a.values.at(2) + b.values.at(2);

    return c;
}
