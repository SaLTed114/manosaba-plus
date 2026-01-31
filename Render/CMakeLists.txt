set(RENDER_SOURCES
    DX11Renderer.cpp
    DX11CommonState.cpp

    Demo/TriangleDemo.cpp
    Demo/CubeDemo.cpp

    Pipelines/SpritePipeline.cpp
    Pipelines/ComposePipeline.cpp
    Pipelines/MeshPipeline.cpp
    Pipelines/PipelineLibrary.cpp

    Shader/ShaderManager.cpp

    Scene3D/Camera3D.cpp

    Passes/SceneSpritePass.cpp
    Passes/ComposePass.cpp
    Passes/CubePass.cpp

    Drawers/DrawServices.cpp
    Drawers/SpriteBatcher.cpp
    Drawers/MeshDrawer.cpp
)

set(RENDER_HEADERS
    DX11Renderer.h
    DX11CommonState.h

    Demo/TriangleDemo.h
    Demo/CubeDemo.h

    Pipelines/SpritePipeline.h
    Pipelines/ComposePipeline.h
    Pipelines/MeshPipeline.h
    Pipelines/PipelineLibrary.h

    Draw/DrawItem.h
    Draw/DrawList.h

    Shader/ShaderManager.h

    Scene3D/Camera3D.h

    Passes/IRenderPass.h
    Passes/RenderPassBase.h
    Passes/SceneSpritePass.h
    Passes/ComposePass.h
    Passes/CubePass.h
    
    Drawers/DrawServices.h
    Drawers/SpriteBatcher.h
    Drawers/MeshDrawer.h
)

set(TRIANGLE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/triangle.hlsl)
set(SPRITE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/sprite.hlsl)
set(COMPOSE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/compose.hlsl)
set(MESH_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/mesh.hlsl)

compile_shader_fxc(${TRIANGLE_SHADER_SOURCE} vs_5_0 VS TRI_VS_CSO)
compile_shader_fxc(${TRIANGLE_SHADER_SOURCE} ps_5_0 PS TRI_PS_CSO)
compile_shader_fxc(${SPRITE_SHADER_SOURCE} vs_5_0 VS SPRITE_VS_CSO)
compile_shader_fxc(${SPRITE_SHADER_SOURCE} ps_5_0 PS SPRITE_PS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} vs_5_0 VS COMPOSE_VS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} ps_5_0 PS COMPOSE_PS_CSO)
compile_shader_fxc(${MESH_SHADER_SOURCE} vs_5_0 VS MESH_VS_CSO)
compile_shader_fxc(${MESH_SHADER_SOURCE} ps_5_0 PS MESH_PS_CSO)

add_library(Render STATIC
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}

    ${TRI_VS_CSO}
    ${TRI_PS_CSO}
    ${SPRITE_VS_CSO}
    ${SPRITE_PS_CSO}
    ${COMPOSE_VS_CSO}
    ${COMPOSE_PS_CSO}
    ${MESH_VS_CSO}
    ${MESH_PS_CSO}
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