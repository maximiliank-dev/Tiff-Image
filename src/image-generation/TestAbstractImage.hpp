#include <memory>

#include "../ImageContainer.hpp"

template <typename T>
class TestAbstractImage {
   public:
    using ImageType = ImageContainer<T>;

   protected:
    std::shared_ptr<ImageType> img;

   public:
    TestAbstractImage(const size_t n, const size_t k)
        : img(std::make_shared<ImageContainer<uint8_t>>(n, k)) {}

    /**
     * Constructs the image according to the
     *
     * */
    virtual void generateImage() = 0;
    const std::shared_ptr<ImageType> get_image() { return this->img; }
};
