// Utils/ConsoleUtils.h
#ifndef UTILS_CONSOLEUTILS_H
#define UTILS_CONSOLEUTILS_H

#include <Windows.h>
#include <cstdio>
#include <iostream>

namespace Salt2D::Utils {

inline bool AttachParentConsoleAndRedirectIO() {
    if (GetConsoleWindow() != nullptr) return true;
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) return false;

    FILE* dummy = nullptr;
    freopen_s(&dummy, "CONOUT$", "w", stdout);
    freopen_s(&dummy, "CONOUT$", "w", stderr);
    freopen_s(&dummy, "CONIN$",  "r", stdin);

    std::ios::sync_with_stdio();
    return true;
}

} // namespace Salt2D::Utils

#endif // UTILS_CONSOLEUTILS_H