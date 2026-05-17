#pragma once

#include <ranges>
#include <vector>
#include <unordered_map>
#include <string>
#include <format>
#include <deque>


#if defined(_MSC_VER)
    #include <intrin.h>
#endif


namespace compression {



// Returns the 0-indexed position of the most significant set bit.
// Returns -1 if x == 0.
inline int msb_position(uint32_t x) noexcept {
    if (x == 0) return -1;

#if defined(__GNUC__)
    return 31 - __builtin_clz(x);
#elif defined(_MSC_VER)
    unsigned long idx;
    _BitScanReverse(&idx, x);
    return static_cast<int>(idx);
#else
    int pos = -1;
    while (x) { ++pos; x >>= 1; }
    return pos;
#endif
}


constexpr uint16_t CLEAR = 256;
constexpr uint16_t EOI   = 257;
constexpr uint16_t FIRST = 258;
constexpr int      MIN_W = 9;
constexpr int      MAX_W = 12;
constexpr uint32_t MAX_SZ = 4096;


/**
 * Class for the LZW compression
 * Implements the compressor and the decompressor
 * This is an implementation for TIFF
 * The dictionary entries 256 and 257 are reserved!
 * 267: ClearCode
 * 268: EndOfInformation
 */
class LZW {

private:

uint8_t compression_data_left;
size_t compression_number_bits_left;


static void init_map_encode(std::unordered_map<std::string, uint16_t>& map) {
    map.clear();
    for (uint16_t i = 0; i < 256; i++) {
        map[std::string(1, static_cast<char>(i))] = i;
    }
}

static void init_map_decode(std::unordered_map<uint16_t, std::string>& map) {
    map.clear();
    for (uint16_t i = 0; i < 256; i++) {
        map[i] = std::string(1, static_cast<char>(i));
    }
}

public:

/**
 * LZW Compressor
 */
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

/**
 * LZW Decompressor
 */
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

/**
 * Appends a LZW compressed data bit to a byte vector
 * Compresses b0b1b2b3b4b5b6b7b8b9 c0c1c2c3c4c5c6c7c8 to:
 *  b0b1b2b3b4b5b6b7 b8b9c0c1c2c3c4c5
 * @Required initialization of compression_data_left compression_number_bits_left member variables
 * 
 */
void append_to_byte_vector(uint16_t data, size_t bit_length, std::vector<uint8_t>& vec ) {
    if(bit_length > 12)
        throw std::runtime_error("Error in append_to_byte_vector bitlength > 12");

    size_t bits_available = (8-this->compression_number_bits_left);

    //cas when more than 8 bits are available
    if(bit_length >= bits_available ) {
        size_t bits_left = bit_length - bits_available;

        vec.push_back( this->compression_data_left + 
                            (data >> bits_left) //remove unused bits
        );

        if(bits_left >= 8) { //in case 
            vec.push_back(
                static_cast<uint8_t>(data >> (bits_left - 8)));
           bits_left -= 8;
        }

        //check if there are unused bits
        this->compression_data_left = (bits_left != 0)?(data << (8-bits_left)):0;
        this->compression_number_bits_left = bits_left;
    } else { 
        this->compression_data_left |= static_cast<uint8_t>(data << (bits_available - bit_length));
        this->compression_number_bits_left += bit_length;
    }
}

/**
 * The queue contains a stream of bits packed in bytes
 * The function decodes them into chunks of bit_length which is the input of the LZW decompressor
 */
uint16_t decode_byte_queue(const size_t bit_length, std::deque<uint8_t>& queue) {
    if(bit_length > 12)
        throw std::runtime_error("Error in decode_byte_vector bitlength > 12");
    if(bit_length < 8)
        throw std::runtime_error("Error in decode_byte_vector bitlength < 8");


    uint16_t data{0};
    int bits_needed = bit_length - this->compression_number_bits_left;


    uint16_t pop = queue.front();
    queue.pop_front();

    if(bits_needed > 8) {
        data = (this->compression_data_left << 8) | pop;
        pop = queue.front();
        queue.pop_front();

        bits_needed -= 8;
        data = (data << bits_needed) | (pop >> (8-bits_needed));
    } else {
        data = (this->compression_data_left << bits_needed) | (pop >> (8-bits_needed));
    }

    this->compression_number_bits_left = (8 - bits_needed);
    uint16_t mask = (1u << this->compression_number_bits_left) - 1;
    this->compression_data_left = pop & mask;

    return data;
}

/**
 * LZW Compressor
 */
std::vector<uint8_t> compress_tiff(std::vector<uint8_t>& c) {
    std::vector<uint8_t> compressed;
    compressed.reserve(2*c.size());

    //init member variables
    this->compression_data_left = 0;
    this->compression_number_bits_left = 0; //important one

    size_t current_bit_length = 9;


    //defined in the standard
    append_to_byte_vector(CLEAR, current_bit_length, compressed);

    //init the map
    std::unordered_map<std::string, uint16_t> map;
    uint16_t idx = 257;
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
                append_to_byte_vector(map.at(sub), current_bit_length, compressed);
            } else { //otherwise write a single character
                append_to_byte_vector(sequence[0], current_bit_length, compressed);
            }

            sequence = {static_cast<char>(element)};

            //increment bit_length if necessary
            if( (msb_position(idx)+1) > current_bit_length) {
                current_bit_length = msb_position(idx)+1;
            }
        }


        if(idx == MAX_SZ) {
            idx = 257;
            init_map_encode(map);
            append_to_byte_vector(CLEAR, current_bit_length, compressed);
            current_bit_length = 9;
        }
    }


    //append the last saved sequence, otherwise the last element is missing
    if (!sequence.empty()) {
        // compressed.push_back(map.at(sequence));
        append_to_byte_vector(map.at(sequence), current_bit_length, compressed);
    }

    //append the last bytes if needed
    append_to_byte_vector(EOI, current_bit_length, compressed);
    if (this->compression_number_bits_left > 0) {
        compressed.push_back(this->compression_data_left);
    }


    return compressed;
};

/**
 * LZW Decompressor
 */
std::vector<uint8_t> decompress_tiff(std::vector<uint8_t>& c) {
    std::vector<uint8_t> decompressed;
    decompressed.reserve(2*c.size());

    if(c.size() == 0)
        throw std::runtime_error("Error LZW decompress: size is zero");

    std::deque<uint8_t> deque(c.begin(), c.end());

    //init member variables
    this->compression_data_left = 0;
    this->compression_number_bits_left = 0; //important one

    size_t current_bit_length = 9;


    //init the map
    std::unordered_map<uint16_t, std::string> map;
    uint16_t idx = 257;
    uint16_t previous = 0;

    auto clear_routine = [&]() -> void {
        idx = 257;
        init_map_decode(map);
        current_bit_length = 9;

        previous = this->decode_byte_queue(current_bit_length, deque);
        for(auto& byte : map.at(previous)) {
            decompressed.push_back(static_cast<uint8_t>(byte));
        }
    };


    uint16_t clear_byte = this->decode_byte_queue(current_bit_length, deque);
    if(clear_byte != CLEAR) {
        throw std::runtime_error("Error expecting a clear bit at the start");
    }
    clear_routine();


    while(!deque.empty()) {
        uint16_t code = this->decode_byte_queue(current_bit_length, deque);
        std::string C{""};
        std::string output{""};

        //decoding finished
        if(code == EOI)
            break;

        //rebuild map
        if(code == CLEAR) {
            clear_routine();
            continue;
        }


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

        //increment bit_length if necessary
        if( (msb_position(idx)+1) > current_bit_length) {
            current_bit_length = msb_position(idx)+1;
        }

    }

    return decompressed;
}

};

};