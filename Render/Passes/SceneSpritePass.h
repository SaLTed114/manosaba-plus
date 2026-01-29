// Render/Passes/SceneSpritePass.h
#ifndef RENDER_PASSES_SCENESPRITEPASS_H
#define RENDER_PASSES_SCENESPRITEPASS_H

#include <span>

#include "RenderPassBase.h"
#include "Render/Draw/DrawList.h"

namespace Salt2D::Render {

class SpritePass : public RenderPassBase {
public:
    SpritePass(
        const char* name, Target target,
        DepthMode depth, BlendMode blend,
        std::span<const SpriteDrawItem> sprites);

    void SetClearScene(float r, float g, float b, float a);

protected:
    void Execute(PassContext& ctx) override;

private:
    std::span<const SpriteDrawItem> sprites_;
};

} // namespace Salt2D::Render

#endif // RENDER_PASSES_SCENESPRITEPASS_H
