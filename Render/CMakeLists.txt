set(RENDER_SOURCES
    DX11Renderer.cpp
)

set(RENDER_HEADERS
    DX11Renderer.h
)

add_library(Render STATIC
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}
)

target_include_directories(Render PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Render PUBLIC
    RHI_DX11
)