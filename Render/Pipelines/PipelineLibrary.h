// Render/Pipeline/PipelineLibrary.h
#ifndef RENDER_PIPELINES_PIPELINELIBRARY_H
#define RENDER_PIPELINES_PIPELINELIBRARY_H

#include <memory>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class ComposePipeline;
class MeshPipeline;
class SpritePipeline;

class PipelineLibrary {
public:
    PipelineLibrary();
    ~PipelineLibrary();
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);

    ComposePipeline& Compose() { return *compose_; }
    MeshPipeline&    Mesh()    { return *mesh_; }
    SpritePipeline&  Sprite()  { return *sprite_; }

private:
    std::unique_ptr<ComposePipeline> compose_;
    std::unique_ptr<MeshPipeline>    mesh_;
    std::unique_ptr<SpritePipeline>  sprite_;
};

} // namespace Salt2D::Render

#endif // RENDER_PIPELINES_PIPELINELIBRARY_H
