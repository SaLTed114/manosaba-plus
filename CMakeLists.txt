cmake_minimum_required(VERSION 3.20)
project(manosaba_plus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor /Zc:inline /EHa")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")

add_subdirectory(Core)
add_subdirectory(App)
add_subdirectory(RHI)
add_subdirectory(Render)