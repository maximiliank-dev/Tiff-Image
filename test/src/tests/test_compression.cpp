#include <gtest/gtest.h>

#include <filesystem>

#include "compression/lzw.hpp"

#include <list>
#include <tuple>

struct TestParameter {
    std::vector<uint8_t> data;
    std::vector<uint16_t> data_compressed;
};

class LZWTest :
    public testing::TestWithParam<TestParameter> {
  // You can implement all the usual fixture class members here.
  // To access the test parameter, call GetParam() from class
  // TestWithParam<T>.

};

TEST_P(LZWTest, BasicAssertions) {

    compression::LZW lzw;

    std::vector<uint8_t> data = GetParam().data;
    std::vector<uint16_t> data_compressed = GetParam().data_compressed;


    std::vector<uint16_t> comp = lzw.compress(data);
    for(size_t i = 0; i < comp.size(); ++i) {
        uint16_t element = comp.at(i);
        if( element < 256) {
            std::cout << static_cast<char>(element);
        } else
            std::cout << i;
    
        std::cout << " ";
        EXPECT_EQ(element, data_compressed.at(i) )  << "Error compression expected " << data_compressed.at(i) << " got " << element << '\n';
    }
    std::cout << "\n\n";  


    std::vector<uint8_t> decomp = lzw.decompress(data_compressed);
    for(size_t i = 0; i < decomp.size(); ++i) {
        uint8_t element = decomp.at(i);
        if( element < 256) {
            std::cout << static_cast<char>(element);
        } else
            std::cout << i;
    
        std::cout << " ";
        EXPECT_EQ(element, data.at(i) )  << "\nError decompression expected " << data.at(i) << " got " << element << " i " << i << '\n';
    }   

};

INSTANTIATE_TEST_SUITE_P(BasicTest,
                         LZWTest,
                         testing::Values(TestParameter{
                            {'L','Z','W','L','Z','7','8','L','Z','7','7','L','Z','C','L','Z','M','W','L','Z','A','P'},
                            {'L', 'Z', 'W', 256, '7', '8', 259, '7', 256, 'C', 256, 'M', 258, 'Z', 'A', 'P'}
                         },
                        TestParameter{
                            {'a', 'a', 'a', 'a'},
                            {'a', 256, 'a'}
                         }              
                        ));