// Utils/IFileSystem.h
#ifndef UTILS_IFILESYSTEM_H
#define UTILS_IFILESYSTEM_H

#include <filesystem>
#include <string>
#include <vector>

namespace Salt2D::Utils {

struct IFileSystem {
    virtual ~IFileSystem() = default;

    virtual bool Exists(const std::filesystem::path& path) const = 0;
    virtual std::string ReadTextFileUtf8(const std::filesystem::path& path, bool normalizeNewLines = true) = 0;
    virtual std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& path) = 0;
};

} // namespace Salt2D::Utils

#endif // UTILS_IFILESYSTEM_H
