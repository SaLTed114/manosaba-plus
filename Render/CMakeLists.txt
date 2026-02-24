set(RENDER_SOURCES
    DX11Renderer.cpp
    DX11CommonState.cpp
    DX11DebugLogger.cpp

    Pipelines/SpritePipeline.cpp
    Pipelines/ComposePipeline.cpp
    Pipelines/MeshPipeline.cpp
    Pipelines/CardPipeline.cpp
    Pipelines/PipelineLibrary.cpp

    Shader/ShaderManager.cpp

    Scene3D/Camera3D.cpp
    Scene3D/MeshFactory.cpp

    Text/TextBaker.cpp

    Passes/SceneSpritePass.cpp
    Passes/ComposePass.cpp
    Passes/MeshPass.cpp
    Passes/CardPass.cpp

    Drawers/DrawServices.cpp
    Drawers/SpriteBatcher.cpp
    Drawers/MeshDrawer.cpp
    Drawers/CardDrawer.cpp
)

set(RENDER_HEADERS
    DX11Renderer.h
    DX11CommonState.h
    DX11DebugLogger.h

    Pipelines/SpritePipeline.h
    Pipelines/ComposePipeline.h
    Pipelines/MeshPipeline.h
    Pipelines/CardPipeline.h
    Pipelines/PipelineLibrary.h

    Draw/SpriteDrawItem.h
    Draw/DrawList.h

    Shader/ShaderManager.h

    Scene3D/Camera3D.h
    Scene3D/MeshFactory.h

    Text/TextBaker.h

    Passes/IRenderPass.h
    Passes/RenderPassBase.h
    Passes/SceneSpritePass.h
    Passes/ComposePass.h
    Passes/MeshPass.h
    Passes/CardPass.h
    
    Drawers/DrawServices.h
    Drawers/SpriteBatcher.h
    Drawers/MeshDrawer.h
    Drawers/CardDrawer.h
)

set(SPRITE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/sprite.hlsl)
set(COMPOSE_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/compose.hlsl)
set(MESH_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/mesh.hlsl)
set(CARD_SHADER_SOURCE ${CMAKE_SOURCE_DIR}/Shaders/card.hlsl)

compile_shader_fxc(${SPRITE_SHADER_SOURCE} vs_5_0 VS SPRITE_VS_CSO)
compile_shader_fxc(${SPRITE_SHADER_SOURCE} ps_5_0 PS SPRITE_PS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} vs_5_0 VS COMPOSE_VS_CSO)
compile_shader_fxc(${COMPOSE_SHADER_SOURCE} ps_5_0 PS COMPOSE_PS_CSO)
compile_shader_fxc(${MESH_SHADER_SOURCE} vs_5_0 VS MESH_VS_CSO)
compile_shader_fxc(${MESH_SHADER_SOURCE} ps_5_0 PS MESH_PS_CSO)
compile_shader_fxc(${CARD_SHADER_SOURCE} vs_5_0 VS CARD_VS_CSO)
compile_shader_fxc(${CARD_SHADER_SOURCE} ps_5_0 PS CARD_PS_CSO)

add_library(Render STATIC
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}

    ${SPRITE_VS_CSO}
    ${SPRITE_PS_CSO}
    ${COMPOSE_VS_CSO}
    ${COMPOSE_PS_CSO}
    ${MESH_VS_CSO}
    ${MESH_PS_CSO}
    ${CARD_VS_CSO}
    ${CARD_PS_CSO}
)

target_include_directories(Render PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(Render PUBLIC
    RHI_DX11
    Utils
    Resources
    d3dcompiler
    dwrite
    d2d1
    windowscodecs
)