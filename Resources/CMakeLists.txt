set(RESOURCES_SOURCES
    Image/WICImageLoader.cpp
)

set(RESOURCES_HEADERS
    Image/WICImageLoader.h
)

add_library(Resources STATIC
    ${RESOURCES_SOURCES}
    ${RESOURCES_HEADERS}
)

target_include_directories(Resources PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Resources PUBLIC
    windowscodecs
)

if(MSVC)
    target_compile_options(Resources PRIVATE /EHsc)
endif()