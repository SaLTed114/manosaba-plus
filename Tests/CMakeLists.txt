# Tests CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# ========================================
# Game/Story Tests
# ========================================

add_executable(StoryGraphLoaderTest
    Game/Story/StoryGraphLoaderTest.cpp
    ${CMAKE_SOURCE_DIR}/Game/Story/StoryGraphLoader.cpp
)

target_include_directories(StoryGraphLoaderTest PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

target_link_libraries(StoryGraphLoaderTest PRIVATE
    Utils
)

set_target_properties(StoryGraphLoaderTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/$<CONFIG>
)

add_executable(StoryRuntimeTest
    Game/Story/StoryRuntimeTest.cpp
    ${CMAKE_SOURCE_DIR}/Game/Story/StoryGraphLoader.cpp
    ${CMAKE_SOURCE_DIR}/Game/Story/StoryRuntime.cpp
)

target_include_directories(StoryRuntimeTest PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

target_link_libraries(StoryRuntimeTest PRIVATE
    Utils
)

set_target_properties(StoryRuntimeTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin/$<CONFIG>
)

# ========================================
# Future Tests
# ========================================

# Add more test executables here as needed

# ========================================
# All Tests Target
# ========================================

# Create a custom target that builds all tests
add_custom_target(Tests
    DEPENDS StoryGraphLoaderTest StoryRuntimeTest
    COMMENT "Building all tests"
)