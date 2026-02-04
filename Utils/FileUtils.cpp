// Utils/FileUtils.cpp
#include "Utils/FileUtils.h"
#include "Windows.h"

namespace Salt2D::Utils {
namespace fs = std::filesystem;

static inline void StripUtf8Bom(std::string& content) {
    const std::string utf8Bom = "\xEF\xBB\xBF";
    if (content.rfind(utf8Bom, 0) == 0) {
        content.erase(0, utf8Bom.size());
    }
}

static inline void NormalizeNewLines(std::string& content) {
    std::string normalized;
    normalized.reserve(content.size());
    for (size_t i = 0; i < content.size(); ++i) {
        if (content[i] == '\r') {
            if (i + 1 < content.size() && content[i + 1] == '\n') ++i;
            normalized += '\n';
        } else {
            normalized += content[i];
        }
    }
    content.swap(normalized);
}

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

fs::path ResolveRelative(const fs::path& baseDir, const fs::path& relOrAbsPath) {
    if (relOrAbsPath.is_absolute()) return relOrAbsPath;
    return fs::weakly_canonical(baseDir / relOrAbsPath);
}

std::string ReadTextFileUtf8(const fs::path& path, bool normalizeNewLines) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) throw std::runtime_error("Failed to open file: " + path.string());

    std::string content(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>()
    );

    StripUtf8Bom(content);
    if (normalizeNewLines) NormalizeNewLines(content);
    return content;
}

std::vector<uint8_t> ReadBinaryFile(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) throw std::runtime_error("Failed to open file: " + path.string());

    ifs.seekg(0, std::ios::end);
    std::streampos fileSize = ifs.tellg();
    if (fileSize < 0) throw std::runtime_error("Failed to determine file size: " + path.string());
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
    if (!buffer.empty()) {
        ifs.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        if (!ifs) throw std::runtime_error("Failed to read file: " + path.string());
    }
    return buffer;
}

std::string ReadTextFileUtf8Resolved(const fs::path& path, int maxLevelsUp, bool normalizeNewLines) {
    fs::path resolvedPath = ResolvePath(path, maxLevelsUp);
    return ReadTextFileUtf8(resolvedPath, normalizeNewLines);
}

std::vector<uint8_t> ReadBinaryFileResolved(const fs::path& path, int maxLevelsUp) {
    fs::path resolvedPath = ResolvePath(path, maxLevelsUp);
    return ReadBinaryFile(resolvedPath);
}

std::string GenerateTimestampedFilename(const std::string& basePath) {
    fs::path p(basePath);
    fs::path dir = p.parent_path();
    fs::path stem = p.stem();
    fs::path ext = p.extension();

    SYSTEMTIME st;
    GetLocalTime(&st);

    // Format timestamp: YYYYMMDD_HHMMSS
    char timestamp[32];
    snprintf(timestamp, sizeof(timestamp),
        "%04d%02d%02d_%02d%02d%02d",
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond);

    // Combine new filename
    fs::path newFilename = stem.string() + "_" + timestamp + ext.string();
    fs::path result = dir / newFilename;

    return result.string();
}

} // namespace Salt2D::Utils
