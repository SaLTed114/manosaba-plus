// Render/Passes/CubePass.h
#ifndef RENDER_PASSES_CUBEPASS_H
#define RENDER_PASSES_CUBEPASS_H

#include "RenderPassBase.h"
#include <DirectXMath.h>

namespace Salt2D::Render {

class CubePass : public RenderPassBase {
public:
    CubePass(const char* name, DirectX::XMMATRIX transform);

protected:
    void Execute(PassContext& ctx) override;

private:
    DirectX::XMMATRIX transform_;
};

} // namespace Salt2D::Render

#endif // RENDER_PASSES_CUBEPASS_H
