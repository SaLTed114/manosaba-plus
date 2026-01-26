set(RENDER_SOURCES
    DX11Renderer.cpp
    DX11CommonState.cpp

    Primitives/TriangleDemo.cpp

    Pipeline/SpritePipeline.cpp
    Pipeline/ComposePipeline.cpp

    Sprite/SpriteRenderer.cpp

    Shader/ShaderManager.cpp
)

set(RENDER_HEADERS
    DX11Renderer.h
    DX11CommonState.h

    Primitives/TriangleDemo.h

    Pipeline/SpritePipeline.h
    Pipeline/ComposePipeline.h

    Sprite/SpriteRenderer.h

    Draw/DrawItem.h
    Draw/DrawList.h

    Shader/ShaderManager.h
)

set(TRIANGLE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/triangle.hlsl)
set(SPRITE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/sprite.hlsl)
set(COMPOSE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/compose.hlsl)

compile_shader_fxc(${TRIANGLE_SHADER_SOURCE} vs_5_0 VS TRI_VS_CSO)
compile_shader_fxc(${TRIANGLE_SHADER_SOURCE} ps_5_0 PS TRI_PS_CSO)
compile_shader_fxc(${SPRITE_SHADER_SOURCE} vs_5_0 VS SPRITE_VS_CSO)
compile_shader_fxc(${SPRITE_SHADER_SOURCE} ps_5_0 PS SPRITE_PS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} vs_5_0 VS COMPOSE_VS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} ps_5_0 PS COMPOSE_PS_CSO)

add_library(Render STATIC
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}

    ${TRI_VS_CSO}
    ${TRI_PS_CSO}
    ${SPRITE_VS_CSO}
    ${SPRITE_PS_CSO}
    ${COMPOSE_VS_CSO}
    ${COMPOSE_PS_CSO}
)

target_include_directories(Render PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Render PUBLIC
    RHI_DX11
    Utils
    Resources
    d3dcompiler
)