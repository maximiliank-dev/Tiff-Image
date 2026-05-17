#pragma once

#include <tiffio.h>
#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace libtiff_wrapper {



struct MemBuf {
    std::vector<uint8_t> data;
    tmsize_t pos = 0;
};

static tmsize_t mem_read(thandle_t h, void* buf, tmsize_t n) {
    auto* m = static_cast<MemBuf*>(h);
    tmsize_t avail = static_cast<tmsize_t>(m->data.size()) - m->pos;
    tmsize_t take = std::min(n, avail);
    std::memcpy(buf, m->data.data() + m->pos, take);
    m->pos += take;
    return take;
}
static tmsize_t mem_write(thandle_t h, void* buf, tmsize_t n) {
    auto* m = static_cast<MemBuf*>(h);
    if (m->pos + n > static_cast<tmsize_t>(m->data.size()))
        m->data.resize(m->pos + n);
    std::memcpy(m->data.data() + m->pos, buf, n);
    m->pos += n;
    return n;
}
static toff_t mem_seek(thandle_t h, toff_t off, int whence) {
    auto* m = static_cast<MemBuf*>(h);
    toff_t newpos = 0;
    switch (whence) {
        case SEEK_SET: newpos = off; break;
        case SEEK_CUR: newpos = m->pos + off; break;
        case SEEK_END: newpos = m->data.size() + off; break;
    }
    if (newpos > static_cast<toff_t>(m->data.size())) m->data.resize(newpos);
    m->pos = newpos;
    return newpos;
}
static int    mem_close(thandle_t) { return 0; }
static toff_t mem_size (thandle_t h) { return static_cast<MemBuf*>(h)->data.size(); }

std::vector<uint8_t> libtiff_lzw_encode(const std::vector<uint8_t>& input) {
    MemBuf mem;
    TIFF* tif = TIFFClientOpen("mem", "w", &mem,
        mem_read, mem_write, mem_seek, mem_close, mem_size, nullptr, nullptr);
    if (!tif) throw std::runtime_error("TIFFClientOpen failed");

    const uint32_t width  = static_cast<uint32_t>(input.size());
    const uint32_t height = 1;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    height);
    TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_LZW);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PREDICTOR,       1);   // no predictor — critical for verification

    if (TIFFWriteEncodedStrip(tif, 0,
            const_cast<uint8_t*>(input.data()), input.size()) < 0)
        throw std::runtime_error("TIFFWriteEncodedStrip failed");

    TIFFClose(tif);

    // Reopen the in-memory TIFF for reading and pull out the raw compressed strip
    mem.pos = 0;
    tif = TIFFClientOpen("mem", "r", &mem,
        mem_read, mem_write, mem_seek, mem_close, mem_size, nullptr, nullptr);
    if (!tif) throw std::runtime_error("TIFFClientOpen (read) failed");

    tmsize_t compressed_size = TIFFRawStripSize(tif, 0);
    std::vector<uint8_t> compressed(compressed_size);
    if (TIFFReadRawStrip(tif, 0, compressed.data(), compressed_size) < 0)
        throw std::runtime_error("TIFFReadRawStrip failed");

    TIFFClose(tif);
    return compressed;
}

std::vector<uint8_t> libtiff_lzw_decode(
    const std::vector<uint8_t>& compressed,
    uint32_t uncompressed_size)
{
    MemBuf mem;
    TIFF* tif = TIFFClientOpen("mem", "w", &mem,
        mem_read, mem_write, mem_seek, mem_close, mem_size, nullptr, nullptr);
    if (!tif) throw std::runtime_error("TIFFClientOpen (write) failed");

    // Describe a 1-row image whose width equals the uncompressed byte count.
    // Metadata must match what the encoder used, or libtiff will reject the strip.
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH,      uncompressed_size);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH,     1u);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,    1u);
    TIFFSetField(tif, TIFFTAG_COMPRESSION,     COMPRESSION_LZW);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PREDICTOR,       1);  // no predictor — must match encoder

    // Inject our compressed bytes directly as the raw strip (no re-encoding).
    if (TIFFWriteRawStrip(tif, 0,
            const_cast<uint8_t*>(compressed.data()),
            static_cast<tmsize_t>(compressed.size())) < 0)
        throw std::runtime_error("TIFFWriteRawStrip failed");

    TIFFClose(tif);  // finalizes the in-memory TIFF (writes header + IFD)

    // Reopen for reading and let libtiff decode the strip through its LZW codec.
    mem.pos = 0;
    tif = TIFFClientOpen("mem", "r", &mem,
        mem_read, mem_write, mem_seek, mem_close, mem_size, nullptr, nullptr);
    if (!tif) throw std::runtime_error("TIFFClientOpen (read) failed");

    tmsize_t strip_size = TIFFStripSize(tif);   // size after decompression
    std::vector<uint8_t> decoded(strip_size);

    tmsize_t got = TIFFReadEncodedStrip(tif, 0, decoded.data(), strip_size);
    if (got < 0) {
        TIFFClose(tif);
        throw std::runtime_error("TIFFReadEncodedStrip failed");
    }
    decoded.resize(got);

    TIFFClose(tif);
    return decoded;
}


};
