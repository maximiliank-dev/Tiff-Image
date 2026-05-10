#include <gtest/gtest.h>

#include <array>
#include <cstdint>

#include "TestRGBImages.hpp"

// prevent the function to be visible in other .cc/.cpp tests
namespace {

    std::shared_ptr<ImageContainer<uint8_t>> make_ramp(size_t rows, size_t cols,
                                                    RampType type,
                                                    std::array<size_t, 3> idx) {
    TestImageRampRGB im(rows, cols, type, 0, 255, idx);
    im.generateImage();
    return im.get_image();
}

}  // namespace

// ---------------------------------------------------------------------------
// Dimensions
// ---------------------------------------------------------------------------

TEST(RampImageTest, CorrectDimensions) {
    auto img = make_ramp(10, 8, RampType::HORIZONTAL, {1, 0, 0});
    EXPECT_EQ(img->get_height(), 10u);
    EXPECT_EQ(img->get_width(), 8u);
}

// ---------------------------------------------------------------------------
// Test if Inactive channels are always zero
// ---------------------------------------------------------------------------
struct InactiveChannelParam {
    RampType type;
    std::array<size_t, 3> idx;
    size_t inactive_ch;
};

class InactiveChannelTest : public testing::TestWithParam<InactiveChannelParam> {};

TEST_P(InactiveChannelTest, IsZero) {
    const auto& p = GetParam();
    auto img = make_ramp(8, 8, p.type, p.idx);
    for (size_t i = 0; i < img->get_height(); ++i) {
        for (size_t j = 0; j < img->get_width(); ++j) {
            EXPECT_EQ(img->at(i, j, p.inactive_ch), uint8_t{0})
                << "channel " << p.inactive_ch << " should be 0 at (" << i << "," << j << ")";
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    Channels, InactiveChannelTest,
    ::testing::Values(
        InactiveChannelParam{RampType::HORIZONTAL, {1, 0, 0}, 1},
        InactiveChannelParam{RampType::HORIZONTAL, {1, 0, 0}, 2},
        InactiveChannelParam{RampType::HORIZONTAL, {0, 1, 0}, 0},
        InactiveChannelParam{RampType::HORIZONTAL, {0, 1, 0}, 2},
        InactiveChannelParam{RampType::HORIZONTAL, {0, 0, 1}, 0},
        InactiveChannelParam{RampType::HORIZONTAL, {0, 0, 1}, 1},
        InactiveChannelParam{RampType::VERTICAL,   {1, 0, 0}, 2},
        InactiveChannelParam{RampType::DIAGONAL,   {0, 0, 1}, 0}
    )
);


// ---------------------------------------------------------------------------
// HORIZONTAL ramp: all pixels in the same row have the same value
// ---------------------------------------------------------------------------
TEST(HorizontalRampTest, RowsAreUniform) {
    auto img = make_ramp(10, 12, RampType::HORIZONTAL, {1, 0, 0});
    for (size_t i = 0; i < img->get_height(); ++i) {
        const uint8_t ref = img->at(i, 0, 0);
        for (size_t j = 1; j < img->get_width(); ++j) {
            EXPECT_EQ(img->at(i, j, 0), ref)
                << "row " << i << ": col " << j << " differs from col 0";
        }
    }
}

// ---------------------------------------------------------------------------
// HORIZONTAL ramp: exact values for a 4-row image
// Value is i * 255/4
// ---------------------------------------------------------------------------
TEST(HorizontalRampTest, ExactValues) {
    auto img = make_ramp(4, 2, RampType::HORIZONTAL, {1, 0, 0});
    EXPECT_EQ(img->at(0, 0, 0), uint8_t{0});    // floor(255 * 0/4)
    EXPECT_EQ(img->at(1, 0, 0), uint8_t{63});   // floor(255 * 1/4)
    EXPECT_EQ(img->at(2, 0, 0), uint8_t{127});  // floor(255 * 2/4)
    EXPECT_EQ(img->at(3, 0, 0), uint8_t{191});  // floor(255 * 3/4)
}

// ---------------------------------------------------------------------------
// VERTICAL ramp: all pixels in the same column have the same value
// Value is j * 255/12
// ---------------------------------------------------------------------------
TEST(VerticalRampTest, ColumnsAreUniform) {
    auto img = make_ramp(12, 10, RampType::VERTICAL, {1, 1, 1});
    for(int k = 0; k < 3; k++) {
        for (size_t j = 0; j < img->get_width(); ++j) {
            const uint8_t ref = img->at(0, j, k);

            for (size_t i = 1; i < img->get_height(); ++i) {
                EXPECT_EQ(img->at(i, j, k), ref)
                    << "col " << j << ": row " << i << " differs from row 0";
            }
        }
    }
}

// ---------------------------------------------------------------------------
// VERTICAL ramp: exact values for a 4-column image
// Value is j * 255/4
// ---------------------------------------------------------------------------
TEST(VerticalRampTest, ExactValues) {
    auto img = make_ramp(2, 4, RampType::VERTICAL, {0, 1, 0});
    EXPECT_EQ(img->at(0, 0, 1), uint8_t{0});   // floor(255 * 0/4)
    EXPECT_EQ(img->at(0, 1, 1), uint8_t{63});  // floor(255 * 1/4)
    EXPECT_EQ(img->at(0, 2, 1), uint8_t{127});  // floor(255 * 2/4)
    EXPECT_EQ(img->at(0, 3, 1), uint8_t{191});  // floor(255 * 3/4)
}

// ---------------------------------------------------------------------------
// DIAGONAL ramp: pixels on the same anti-diagonal (i+j = const) have equal values
// AntiDiagonal is i+j
// fxy(i,j) = (i + j) / (max_i + max_j)
// ---------------------------------------------------------------------------

TEST(DiagonalRampTest, AntiDiagonalIsUniform) {
    auto img = make_ramp(8, 8, RampType::DIAGONAL, {1, 0, 0});
    const size_t H = img->get_height();
    const size_t W = img->get_width();

    // A matrix has H+W-1 diagonals -> also so many anti diagonals
    // iterate over every anti diagonal
    // anti diagonal 0 is on the top left 
    for (size_t s = 0; s < H + W - 1; ++s) {
        uint8_t ref = 0;
        bool first = true; // safe the first value

        // iterate over every element in each valid row of the anti diagonal
        for (size_t row = 0; row < H; ++row) {
            // column elements are from   [0,s]         if s<W and s < H,  
            //                            [0, W-1]      if s >= W and s < H
            //                            [s-H+1, s]    if s < W and s >= H
            //                            [s-H+1, W-1]  if s >= W and s >= H
            if (row > s || s - row >= W) continue;
            const size_t col = s - row;
            const uint8_t val = img->at(row, col, 0);

            // compute the first value of the anti diagonal
            if (first) {
                ref = val;
                first = false;
            } else { //check if the other values are the same on the anti diagonal as with the first
                EXPECT_EQ(val, ref)
                    << "anti-diagonal row+col=" << s << " differs at (" << row << "," << col << ")";
            }
        }
    }
}


// ---------------------------------------------------------------------------
// Multiple active channels produce the same values at every pixel
// ---------------------------------------------------------------------------
TEST(RampImageTest, MultipleActiveChannelsAreEqual) {
    for (RampType type : {RampType::HORIZONTAL, RampType::VERTICAL, RampType::DIAGONAL}) {
        auto img = make_ramp(10, 10, type, {1, 1, 1});
        for (size_t i = 0; i < img->get_height(); ++i) {
            for (size_t j = 0; j < img->get_width(); ++j) {
                EXPECT_EQ(img->at(i, j, 0), img->at(i, j, 1))
                    << "channels 0 and 1 differ at (" << i << "," << j << ")";
                EXPECT_EQ(img->at(i, j, 1), img->at(i, j, 2))
                    << "channels 1 and 2 differ at (" << i << "," << j << ")";
            }
        }
    }
}
