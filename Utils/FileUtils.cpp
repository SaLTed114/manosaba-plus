// Utils/FileUtils.cpp
#include "Utils/FileUtils.h"
#include "Windows.h"

namespace Salt2D::Utils {
namespace fs = std::filesystem;

fs::path GetExeDir() {
    wchar_t buffer[MAX_PATH];
    DWORD length = GetModuleFileNameW(NULL, buffer, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        throw std::runtime_error("Failed to get executable path");
    }
    fs::path exePath(buffer);
    return exePath.parent_path();
}

fs::path GetProjectRootDir() {
    static std::optional<fs::path> cachedRoot;
    if (cachedRoot) return *cachedRoot;

    auto exeDir = GetExeDir();
    if (auto root = FindUpward(exeDir, "readme.md")) {
        cachedRoot = root->parent_path();
        return *cachedRoot;
    }

    auto cwd = fs::current_path();
    if (auto root = FindUpward(cwd, "readme.md")) {
        cachedRoot = root->parent_path();
        return *cachedRoot;
    }

    throw std::runtime_error("Project root directory not found");
}

std::optional<fs::path> FindUpward(
    const fs::path& startDir,
    const fs::path& marker,
    int maxLevelsUp
) {
    fs::path currentDir = startDir;
    for (int i = 0; i <= maxLevelsUp; ++i) {
        fs::path candidate = currentDir / marker;
        if (fs::exists(candidate)) return currentDir;
        if (!currentDir.has_parent_path()) break;
        currentDir = currentDir.parent_path();
    }
    return std::nullopt;
}

std::optional<fs::path> TryResolvePath(const fs::path& path, int maxLevelsUp) {
    if (path.is_absolute()) {
        if (fs::exists(path)) return path;
        return std::nullopt;
    }

    // Fast path: try exe directory first (for release builds)
    fs::path exeRelative = GetExeDir() / path;
    if (fs::exists(exeRelative)) return exeRelative;

    // search from cwd first
    {
        fs::path currentPath = fs::current_path();
        for (int i = 0; i <= maxLevelsUp; ++i) {
            fs::path candidatePath = currentPath / path;
            if (fs::exists(candidatePath)) return candidatePath;
            if (!currentPath.has_parent_path()) break;
            currentPath = currentPath.parent_path();
        }
    }

    // search from exe dir next
    {
        fs::path currentPath = GetExeDir();
        for (int i = 0; i <= maxLevelsUp; ++i) {
            fs::path candidatePath = currentPath / path;
            if (fs::exists(candidatePath)) return candidatePath;
            if (!currentPath.has_parent_path()) break;
            currentPath = currentPath.parent_path();
        }
    }

    // search from project root last
    try {
        fs::path projectRoot = GetProjectRootDir();
        fs::path candidatePath = projectRoot / path;
        if (fs::exists(candidatePath)) return candidatePath;
    } catch (const std::runtime_error&) {
        // Ignore if project root cannot be determined
    }
    
    return std::nullopt;
}

fs::path ResolvePath(const fs::path& path, int maxLevelsUp) {
    if (auto resolvedPath = TryResolvePath(path, maxLevelsUp)) return *resolvedPath;
    throw std::runtime_error("Could not resolve path: " + path.string());
}

} // namespace SaltRT::Utils