// Render/Primitives/TestPattern.h
#ifndef RENDER_PRIMITIVES_TESTPATTERN_H
#define RENDER_PRIMITIVES_TESTPATTERN_H

#include <cstdint>
#include <vector>

namespace Salt2D::Render {

static std::vector<uint8_t> MakeCheckerRGBA(uint32_t w, uint32_t h) {
    std::vector<uint8_t> img(w * h * 4);
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            bool c = ((x / 16) ^ (y / 16)) & 1;
            uint8_t v = c ? 255 : 30;

            size_t idx = (y * w + x) * 4;
            img[idx + 0] = v;
            img[idx + 1] = (uint8_t)(x * 255 / (w - 1));
            img[idx + 2] = (uint8_t)(y * 255 / (h - 1));
            img[idx + 3] = 255;
        }
    }
    return img;
}

} // namespace Salt2D::Render

#endif // RENDER_PRIMITIVES_TESTPATTERN_H
