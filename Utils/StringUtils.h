// Utils/StringUtils.h
#ifndef UTILS_STRINGUTILS_H
#define UTILS_STRINGUTILS_H

#include <cmath>
#include <string>
#include <Windows.h>

namespace Salt2D::Utils {

inline std::wstring Utf8ToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

inline size_t Utf8FirstCpBytes(const std::string& str) {
    if (str.empty()) return 0;
    unsigned char c = static_cast<unsigned char>(str[0]);
    if ((c & 0x80) == 0x00) return 1; // 1-byte sequence (ASCII)
    if ((c & 0xE0) == 0xC0) return (str.size() >= 2) ? 2 : 1; // 2-byte sequence
    if ((c & 0xF0) == 0xE0) return (str.size() >= 3) ? 3 : 1; // 3-byte sequence
    if ((c & 0xF8) == 0xF0) return (str.size() >= 4) ? 4 : 1; // 4-byte sequence
    return 1; // Invalid UTF-8, treat as single byte
}

inline void SplitFirstCp(const std::string& str, std::string& firstCp, std::string& rest) {
    size_t cpBytes = Utf8FirstCpBytes(str);
    firstCp = str.substr(0, cpBytes);
    rest    = str.substr(cpBytes);
}

inline void SplitNameFamilyGiven(const std::string& fullName, std::string& familyName, std::string& givenName) {
    // split by space (Chinese / Japanese style)
    size_t spacePos = fullName.find(' ');
    if (spacePos == std::string::npos) { familyName = fullName; givenName = ""; return; }

    familyName = fullName.substr(0, spacePos);

    size_t i = spacePos + 1;
    while (i < fullName.size() && fullName[i] == ' ') ++i; // skip extra spaces
    givenName  = fullName.substr(i);
}

inline std::string FormatMMSS(float sec) {
    if (!std::isfinite(sec)) sec = 0.0f;
    sec = (std::max)(0.0f, sec);
    
    int totalSec = static_cast<int>(std::floor(sec + 1e-6f)); // round to nearest second
    int minutes = totalSec / 60;
    int seconds = totalSec % 60;

    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
    return std::string(buffer);
}

} // namespace Salt2D::Utils

#endif // UTILS_STRINGUTILS_H
