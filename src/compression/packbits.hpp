#pragma once

#include <ranges>
#include <vector>

namespace compression {


class PackBits {

public:

// template<std::ranges::range Container>
static std::vector<uint8_t> compress(std::vector<uint8_t>& c) {
    std::vector<uint8_t> compressed;

    int8_t cnt = 1;
    uint8_t last_byte = *std::ranges::cbegin(c);


    for(auto it = (std::ranges::cbegin(c)+1); it != std::ranges::cend(c); it++) {

        // corner case: run of 127 identical bytes — flush and restart
        if(last_byte == *it && cnt == 127) {
            compressed.push_back(static_cast<uint8_t>(static_cast<int8_t>(1 - cnt)));
            compressed.push_back(last_byte);
            cnt = 1;
            last_byte = *it;
        } else if(last_byte == *it) {
            cnt++;
        } else {
            compressed.push_back(static_cast<uint8_t>(static_cast<int8_t>(1 - cnt)));
            compressed.push_back(last_byte);
            cnt = 1;
            last_byte = *it;
        }
    }

    // Bug 1 fix: flush the final run (was never emitted before)
    compressed.push_back(static_cast<uint8_t>(static_cast<int8_t>(1 - cnt)));
    compressed.push_back(last_byte);

    return compressed;
};

/**
 * Decompression function for the PackBits Compression
 */
static std::vector<uint8_t> decompress(std::vector<uint8_t>& c) {
    std::vector<uint8_t> decompressed;
    decompressed.reserve(2*c.size());

    auto it = std::ranges::cbegin(c);

    while(it != std::ranges::cend(c)) {
        int8_t cnt = reinterpret_cast<const int8_t&>(*it);
        
        it++;

        if(it == std::ranges::cend(c)) {
            throw std::runtime_error("Error end of the iterator reached");
        }

        uint8_t byte = *it;
        if(cnt > 0) { // read the next cnt bytes
            int16_t cnt_long = static_cast<int16_t>(cnt);
            cnt_long++;

            decompressed.push_back(byte);

            for(int16_t i = 1; i < cnt_long; i++) {
                    it++;
                    if(it == std::ranges::cend(c)) {
                        throw std::runtime_error("Error end of the iterator reached");
                    }
                    decompressed.push_back(*it);
            }
        } else if(cnt > -128) {  //append a byte cnt+1 times

            for(int8_t i = 0; i < cnt; i++) {
                decompressed.push_back(byte);
            }
        } else {
            //NOP
        }

        it++;
    }

}

};

};