cmake_minimum_required(VERSION 3.20)
project(manosaba_plus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /permissive- /Zc:__cplusplus /Zc:preprocessor /Zc:inline /EHa")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1")

# ===========================
# FXC shader compiler helper (DX11 DXBC)
# ===========================

function(compile_shader_fxc SHADER_FILE PROFILE ENTRY OUTPUT_FILE)
    get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)
    set(OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR}/Shaders/${FILE_NAME}.cso)

    add_custom_command(
        OUTPUT ${OUTPUT_PATH}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/Shaders
        COMMAND fxc /nologo
            /T ${PROFILE}
            /E ${ENTRY}
            /Fo ${OUTPUT_PATH}
            ${SHADER_FILE}
        DEPENDS ${SHADER_FILE}
        COMMENT "FXC compiling ${SHADER_FILE} (${PROFILE})"
    )

    set(${OUTPUT_FILE} ${OUTPUT_PATH} PARENT_SCOPE)
endfunction()

add_subdirectory(Core)
add_subdirectory(App)
add_subdirectory(RHI)
add_subdirectory(Render)