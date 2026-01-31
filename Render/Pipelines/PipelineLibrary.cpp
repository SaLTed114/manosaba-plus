// Render/Pipelines/PipelineLibrary.cpp
#include "PipelineLibrary.h"
#include "ComposePipeline.h"
#include "MeshPipeline.h"
#include "SpritePipeline.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Render {

PipelineLibrary::PipelineLibrary()  = default;
PipelineLibrary::~PipelineLibrary() = default;

void PipelineLibrary::Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager) {
    compose_ = std::make_unique<ComposePipeline>();
    compose_->Initialize(device, shaderManager);

    mesh_ = std::make_unique<MeshPipeline>();
    mesh_->Initialize(device, shaderManager);

    sprite_ = std::make_unique<SpritePipeline>();
    sprite_->Initialize(device, shaderManager);
}

} // namespace Salt2D::Render