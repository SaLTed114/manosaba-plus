// Utils/DiskFileSystem.cpp
#include "DiskFileSystem.h"
#include "FileUtils.h"

namespace Salt2D::Utils {

bool DiskFileSystem::Exists(const std::filesystem::path& path) const {
    return std::filesystem::exists(path);
}

std::string DiskFileSystem::ReadTextFileUtf8(const std::filesystem::path& path, bool normalizeNewLines) {
    return ReadTextFileUtf8(path, normalizeNewLines);
}

std::vector<uint8_t> DiskFileSystem::ReadBinaryFile(const std::filesystem::path& path) {
    return ReadBinaryFile(path);
}

} // namespace Salt2D::Utils
