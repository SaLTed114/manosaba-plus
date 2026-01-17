// RHI/DX11/DX11Common.h
#ifndef RHI_DX11_COMMON_H
#define RHI_DX11_COMMON_H

#include <stdexcept>
#include <d3d11.h>
#include <cstdio>

inline void ThrowIfFailed(HRESULT hr, const char* what = "") {
    if (FAILED(hr)) {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "HRESULT failed: 0x%08X. %s", hr, what);
        throw std::runtime_error(buffer);
    }
}

#endif // RHI_DX11_COMMON_H
