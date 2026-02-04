// Utils/FileUtils.h
#ifndef UTILS_FILEUTILS_H
#define UTILS_FILEUTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <optional>

namespace Salt2D::Utils {

inline bool StartsWith(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

inline void Trim(std::string& str) {
    auto notSpace = [](unsigned char ch) { return !std::isspace(ch); };
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), notSpace));
    str.erase(std::find_if(str.rbegin(), str.rend(), notSpace).base(), str.end());
}

std::filesystem::path GetExeDir();

std::optional<std::filesystem::path> FindUpward(
    const std::filesystem::path& startDir,
    const std::filesystem::path& marker,
    int maxLevelsUp = 10
);

std::filesystem::path GetProjectRootDir();

std::optional<std::filesystem::path> TryResolvePath(
    const std::filesystem::path& path,
    int maxLevelsUp = 10
);

std::filesystem::path ResolvePath(
    const std::filesystem::path& path,
    int maxLevelsUp = 10
);

std::filesystem::path ResolveRelative(
    const std::filesystem::path& baseDir,
    const std::filesystem::path& relOrAbsPath
);

std::string ReadTextFileUtf8(
    const std::filesystem::path& path,
    bool normalizeNewLines = true
);

std::vector<uint8_t> ReadBinaryFile(
    const std::filesystem::path& path
);

std::string ReadTextFileUtf8Resolved(
    const std::filesystem::path& path,
    int maxLevelsUp = 10,
    bool normalizeNewLines = true
);

std::vector<uint8_t> ReadBinaryFileResolved(
    const std::filesystem::path& path,
    int maxLevelsUp = 10
);

std::string GenerateTimestampedFilename(
    const std::string& basePath
);

} // namespace Salt2D::Utils

#endif // UTILS_FILEUTILS_H
