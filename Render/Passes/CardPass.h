// Render/Passes/CardPass.h

#include "RenderPassBase.h"
#include "Render/Draw/CardDrawItem.h"
#include <span>

namespace Salt2D::Render {

class CardPass : public RenderPassBase {
public:
    CardPass(
        const char* name, Target target,
        DepthMode depth, BlendMode blend,
        std::span<const CardDrawItem> cards);

    void SetClearDepth(float depth = 1.0f, uint8_t stencil = 0);

protected:
    void Execute(PassContext& ctx) override;

private:
    std::span<const CardDrawItem> cards_;

};
    
} // namespace Salt2D::Render
