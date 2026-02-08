#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <optional>

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
// Inactive channels are always zero
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
// First pixel (0, 0) is zero for all ramp types
// px(0,0) = 0*cos(theta) + 0*sin(theta) = 0, pmin = 0 => fxy = 0
// ---------------------------------------------------------------------------

TEST(RampImageTest, FirstPixelIsZero) {
    for (RampType type : {RampType::HORIZONTAL, RampType::VERTICAL, RampType::DIAGONAL}) {
        auto img = make_ramp(8, 8, type, {1, 1, 1});
        for (size_t ch = 0; ch < 3; ++ch) {
            EXPECT_EQ(img->at(0, 0, ch), uint8_t{0})
                << "channel " << ch << " at (0,0) should be 0";
        }
    }
}

// ---------------------------------------------------------------------------
// HORIZONTAL ramp: all pixels in the same row have the same value
// fxy(i,j) = i / max_i  — no j dependency at theta=0
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
// HORIZONTAL ramp: values are non-decreasing along rows (increasing i)
// ---------------------------------------------------------------------------

TEST(HorizontalRampTest, MonotonicAlongRows) {
    auto img = make_ramp(20, 8, RampType::HORIZONTAL, {1, 0, 0});
    for (size_t j = 0; j < img->get_width(); ++j) {
        for (size_t i = 1; i < img->get_height(); ++i) {
            EXPECT_GE(img->at(i, j, 0), img->at(i - 1, j, 0))
                << "col " << j << ": row " << i << " < row " << (i - 1);
        }
    }
}

// ---------------------------------------------------------------------------
// HORIZONTAL ramp: exact values for a 4-row image
//   max_i = 4,  fxy(i) = i / 4
//   value(i) = floor(255 * i / 4)
//     i=0 → 0,  i=1 → 63,  i=2 → 127,  i=3 → 191
// ---------------------------------------------------------------------------

TEST(HorizontalRampTest, ExactValues) {
    auto img = make_ramp(4, 2, RampType::HORIZONTAL, {1, 0, 0});
    EXPECT_EQ(img->at(0, 0, 0), uint8_t{0});    // floor(255 * 0/4)   = 0
    EXPECT_EQ(img->at(1, 0, 0), uint8_t{63});   // floor(255 * 1/4)   = floor(63.75)  = 63
    EXPECT_EQ(img->at(2, 0, 0), uint8_t{127});  // floor(255 * 2/4)   = floor(127.5)  = 127
    EXPECT_EQ(img->at(3, 0, 0), uint8_t{191});  // floor(255 * 3/4)   = floor(191.25) = 191
}

// ---------------------------------------------------------------------------
// VERTICAL ramp: all pixels in the same column have the same value
// fxy(i,j) = j / max_j  — no i dependency at theta=pi/2
// ---------------------------------------------------------------------------

TEST(VerticalRampTest, ColumnsAreUniform) {
    auto img = make_ramp(12, 10, RampType::VERTICAL, {0, 1, 0});
    for (size_t j = 0; j < img->get_width(); ++j) {
        const uint8_t ref = img->at(0, j, 1);
        for (size_t i = 1; i < img->get_height(); ++i) {
            EXPECT_EQ(img->at(i, j, 1), ref)
                << "col " << j << ": row " << i << " differs from row 0";
        }
    }
}

// ---------------------------------------------------------------------------
// VERTICAL ramp: values are non-decreasing along columns (increasing j)
// ---------------------------------------------------------------------------

TEST(VerticalRampTest, MonotonicAlongCols) {
    auto img = make_ramp(8, 20, RampType::VERTICAL, {0, 1, 0});
    for (size_t i = 0; i < img->get_height(); ++i) {
        for (size_t j = 1; j < img->get_width(); ++j) {
            EXPECT_GE(img->at(i, j, 1), img->at(i, j - 1, 1))
                << "row " << i << ": col " << j << " < col " << (j - 1);
        }
    }
}

// ---------------------------------------------------------------------------
// VERTICAL ramp: exact values for a 4-column image
//   max_j = 4 * 3 = 12,  fxy(j) = j / 12   (j is column index 0..3)
//   value(j) = floor(255 * j / 12)
//     j=0 → 0,  j=1 → 21,  j=2 → 42,  j=3 → 63
// ---------------------------------------------------------------------------

TEST(VerticalRampTest, ExactValues) {
    auto img = make_ramp(2, 4, RampType::VERTICAL, {0, 1, 0});
    EXPECT_EQ(img->at(0, 0, 1), uint8_t{0});   // floor(255 * 0/12) = 0
    EXPECT_EQ(img->at(0, 1, 1), uint8_t{21});  // floor(255 * 1/12) = floor(21.25)  = 21
    EXPECT_EQ(img->at(0, 2, 1), uint8_t{42});  // floor(255 * 2/12) = floor(42.5)   = 42
    EXPECT_EQ(img->at(0, 3, 1), uint8_t{63});  // floor(255 * 3/12) = floor(63.75)  = 63
}

// ---------------------------------------------------------------------------
// DIAGONAL ramp: pixels on the same anti-diagonal (i+j = const) have equal values
// fxy(i,j) = (i + j) / (max_i + max_j)
// ---------------------------------------------------------------------------

TEST(DiagonalRampTest, AntiDiagonalIsUniform) {
    auto img = make_ramp(8, 8, RampType::DIAGONAL, {1, 0, 0});
    const size_t H = img->get_height();
    const size_t W = img->get_width();
    for (size_t s = 0; s < H + W - 1; ++s) {
        std::optional<uint8_t> ref;
        for (size_t i = 0; i < H; ++i) {
            if (s < i) break;
            const size_t j = s - i;
            if (j >= W) continue;
            const uint8_t val = img->at(i, j, 0);
            if (!ref.has_value()) {
                ref = val;
            } else {
                EXPECT_EQ(val, *ref)
                    << "anti-diagonal i+j=" << s << " differs at (" << i << "," << j << ")";
            }
        }
    }
}

// ---------------------------------------------------------------------------
// DIAGONAL ramp: values are non-decreasing as i+j increases
// ---------------------------------------------------------------------------

TEST(DiagonalRampTest, MonotonicAlongDiagonal) {
    auto img = make_ramp(8, 8, RampType::DIAGONAL, {1, 0, 0});
    // Walk the main diagonal (i == j): i+j = 2*i, strictly increasing
    for (size_t k = 1; k < img->get_height(); ++k) {
        EXPECT_GE(img->at(k, k, 0), img->at(k - 1, k - 1, 0))
            << "diagonal position " << k << " < " << (k - 1);
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
