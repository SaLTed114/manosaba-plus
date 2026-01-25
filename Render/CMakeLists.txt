set(RENDER_SOURCES
    DX11Renderer.cpp

    Primitives/TriangleDemo.cpp
    Primitives/FullScreenTexDemo.cpp

    Resources/Texture2D.cpp

    Shader/ShaderManager.cpp
)

set(RENDER_HEADERS
    DX11Renderer.h

    Primitives/TriangleDemo.h
    Primitives/FullScreenTexDemo.h

    Resources/Texture2D.h

    Shader/ShaderManager.h
)

set(TRIANGLE_VS_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/triangle_vs.hlsl)
set(TRIANGLE_PS_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/triangle_ps.hlsl)
set(FULLSCREEN_TEX_VS_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/fullscreen_tex_vs.hlsl)
set(FULLSCREEN_TEX_PS_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/fullscreen_tex_ps.hlsl)

compile_shader_fxc(${TRIANGLE_VS_SHADER_SOURCE} vs_5_0 VS TRI_VS_CSO)
compile_shader_fxc(${TRIANGLE_PS_SHADER_SOURCE} ps_5_0 PS TRI_PS_CSO)
compile_shader_fxc(${FULLSCREEN_TEX_VS_SHADER_SOURCE} vs_5_0 VS FULLSCREEN_TEX_VS_CSO)
compile_shader_fxc(${FULLSCREEN_TEX_PS_SHADER_SOURCE} ps_5_0 PS FULLSCREEN_TEX_PS_CSO)

add_library(Render STATIC
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}

    ${TRI_VS_CSO}
    ${TRI_PS_CSO}
    ${FULLSCREEN_TEX_VS_CSO}
    ${FULLSCREEN_TEX_PS_CSO}
)

target_include_directories(Render PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Render PUBLIC
    RHI_DX11
    d3dcompiler
)