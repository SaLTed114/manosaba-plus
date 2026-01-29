// Render/Passes/ComposePass.h
#ifndef RENDER_PASSES_COMPOSEPASS_H
#define RENDER_PASSES_COMPOSEPASS_H

#include "RenderPassBase.h"

namespace Salt2D::Render {

class ComposePass : public RenderPassBase {
public:
    ComposePass(const char* name);

protected:
    void Execute(PassContext& ctx) override;
};

} // namespace Salt2D::Render

#endif // RENDER_PASSES_COMPOSEPASS_H
