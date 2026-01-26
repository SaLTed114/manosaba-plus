// Resources/Image/WICImageLoader.h
#ifndef RESOURCES_IMAGE_WICIMAGELOADER_H
#define RESOURCES_IMAGE_WICIMAGELOADER_H

#include <cstdint>
#include <filesystem>
#include <vector>

namespace Salt2D::Resources {

struct ImageRGBA8 {
    uint32_t width;
    uint32_t height;
    uint32_t rowPitch;
    std::vector<uint8_t> pixels; // RGBA8, row-major, width * height * 4 bytes
};

bool LoadImageRGBA8_WIC(const std::filesystem::path& filepath, ImageRGBA8& outImage);

} // namespace Salt2D::Resources

#endif // RESOURCES_IMAGE_WICIMAGELOADER_H
