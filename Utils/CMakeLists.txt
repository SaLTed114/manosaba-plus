set(UTILS_SOURCES
    FileUtils.cpp
)

set(UTILS_HEADERS
    ConsoleUtils.h
    FileUtils.h
)

add_library(Utils STATIC
    ${UTILS_SOURCES}
    ${UTILS_HEADERS}
)

target_include_directories(Utils PUBLIC
    ${CMAKE_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(Utils PRIVATE /EHsc)
endif()
