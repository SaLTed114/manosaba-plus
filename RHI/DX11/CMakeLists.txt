set(DX11_SOURCES
    DX11Device.cpp
    DX11SwapChain.cpp
    DX11Texture2D.cpp
)

set(DX11_HEADERS
    DX11Device.h
    DX11SwapChain.h
    DX11Texture2D.h
    DX11Common.h
)

add_library(RHI_DX11 STATIC
    ${DX11_SOURCES}
    ${DX11_HEADERS}
)

target_include_directories(RHI_DX11 PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(RHI_DX11 PUBLIC
    dxgi
    d3d11
)