// Utils/DiskFileSystem.h
#ifndef UTILS_DISKFILESYSTEM_H
#define UTILS_DISKFILESYSTEM_H

#include "IFileSystem.h"

namespace Salt2D::Utils {
    
class DiskFileSystem : public IFileSystem {
public:
    bool Exists(const std::filesystem::path& path) const override;
    std::string ReadTextFileUtf8(const std::filesystem::path& path, bool normalizeNewLines = true) override;
    std::vector<uint8_t> ReadBinaryFile(const std::filesystem::path& path) override;
};

} // namespace Salt2D::Utils

#endif // UTILS_DISKFILESYSTEM_H