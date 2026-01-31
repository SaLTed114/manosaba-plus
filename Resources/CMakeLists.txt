set(RESOURCES_SOURCES
    Image/WICImageLoader.cpp
    Mesh/MeshLoader.cpp
)

set(RESOURCES_HEADERS
    Image/WICImageLoader.h
    Mesh/MeshData.h
    Mesh/MeshLoader.h
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
    Utils
)

if(MSVC)
    target_compile_options(Resources PRIVATE /EHsc)
endif()