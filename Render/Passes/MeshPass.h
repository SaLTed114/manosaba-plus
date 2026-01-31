// Render/Passes/MeshPass.h
#ifndef RENDER_PASSES_MESHPASS_H
#define RENDER_PASSES_MESHPASS_H

#include "RenderPassBase.h"
#include "Render/Draw/MeshDrawItem.h"
#include <span>

namespace Salt2D::Render {

class MeshPass : public RenderPassBase {
public:
    MeshPass(
        const char* name,
        Target target,
        DepthMode depth,
        BlendMode blend,
        std::span<const MeshDrawItem> meshes
    );

    void SetClearDepth(float depth = 1.0f, uint8_t stencil = 0);

protected:
    void Execute(PassContext& ctx) override;

private:
    std::span<const MeshDrawItem> meshes_;
};

} // namespace Salt2D::Render

#endif // RENDER_PASSES_MESHPASS_H
