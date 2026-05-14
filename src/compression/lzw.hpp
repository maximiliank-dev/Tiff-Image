#pragma once

#include <ranges>
#include <vector>
#include <unordered_map>
#include <string>
#include <format>

namespace compression {

/**
 * Class for the LZW compression
 * Implements the compressor and the decompressor
 * This is an implementation for TIFF
 * The dictionary entries 256 and 257 are reserved!
 * 267: ClearCode
 * 268: EndOfInformation
 */
class LZW {


public:

static void init_map_encode(std::unordered_map<std::string, uint16_t>& map) {
    for (uint16_t i = 0; i < 256; i++) {
        map[std::string(1, static_cast<char>(i))] = i;
    }
}

static void init_map_decode(std::unordered_map<uint16_t, std::string>& map) {
    for (uint16_t i = 0; i < 256; i++) {
        map[i] = std::string(1, static_cast<char>(i));
    }
}

// template<std::ranges::range Container>
static std::vector<uint16_t> compress(std::vector<uint8_t>& c) {
    std::vector<uint16_t> compressed;
    compressed.reserve(2*c.size());

    //init the map
    std::unordered_map<std::string, uint16_t> map;
    uint16_t idx = 255;
    init_map_encode(map);

    std::string sequence = "";
    for(auto it = std::ranges::cbegin(c); it != std::ranges::cend(c); it++) {
        
        uint8_t element = *it;
        sequence += element;
        // check if element already exists in the map
        if(map.count(sequence) > 0 ) {
            continue;
        } else {
            // store the sequence in the dictionary
            std::string sub = sequence.substr(0, sequence.size() - 1);
            map[sequence] = ++idx;

            //if sequence has more than 1 character 
            // then it should be emitted and written to the map
            if(sequence.size() > 1) {
                compressed.push_back(map.at(sub));
            } else { //otherwise write a single character
                compressed.push_back(sequence[0]);
            }

            sequence = {static_cast<char>(element)};
        }
    }

    //append the last saved sequence, otherwise the last element is missing
    if (!sequence.empty())
        compressed.push_back(map.at(sequence));

    return compressed;
};

//according to pseudocode
static std::vector<uint8_t> decompress(std::vector<uint16_t>& c) {
    std::vector<uint8_t> decompressed;
    decompressed.reserve(2*c.size());

    if(c.size() == 0)
        throw std::runtime_error("Error LZW decompress: size is zero");

    //init the map
    std::unordered_map<uint16_t, std::string> map;
    uint16_t idx = 255;
    init_map_decode(map);

    uint16_t previous = *std::ranges::cbegin(c);
    for(auto& byte : map.at(previous)) {
        decompressed.push_back(static_cast<uint8_t>(byte));
    }

    for(auto it = std::ranges::cbegin(c)+1; it != std::ranges::cend(c); ++it) {
        uint16_t code = *it;
        std::string C{""};
        std::string output{""};


        if(map.count(code) > 0) {
            // code is found in the map
            std::string new_ = map.at(previous) + map.at(code)[0];
            map[++idx] = new_;
        } else {
            // code is not found in the map. 
            // can only happen when the same character appears multiple times in a row
            std::string new_ = map.at(previous) +  map.at(previous)[0];
            map[++idx] = new_;
        }

        //output
        for(auto& byte : map.at(code))
            decompressed.push_back(static_cast<uint8_t>(byte));
        
        previous = code;
    }

    return decompressed;
}

};

};