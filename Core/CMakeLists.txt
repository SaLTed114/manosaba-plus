set(SALT2D_CORE_SOURCES
    Window/Win32Window.cpp
)

set(SALT2D_CORE_HEADERS
    Window/Win32Window.h
)

add_library(Core STATIC
    ${SALT2D_CORE_SOURCES}
    ${SALT2D_CORE_HEADERS}
)

target_include_directories(Core PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(Core PUBLIC UNICODE _UNICODE)

if(MSVC)
    target_compile_options(Core PRIVATE /EHsc)
endif()
