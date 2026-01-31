// Render/Drawers/CardDrawer.h
#ifndef RENDER_DRAWERS_CARDDRAWER_H
#define RENDER_DRAWERS_CARDDRAWER_H

#include <span>
#include <wrl/client.h>
#include <d3d11.h>

#include "Render/Draw/CardDrawItem.h"
#include "Render/RenderPlan.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class CardDrawer {
public:
    void Initialize(const RHI::DX11::DX11Device& device);

    void DrawBatch(PassContext& ctx, std::span<const CardDrawItem> cards);

private:
    struct CardVertex {
        float pos[2];
        float uv[2];
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib_;
    
};

} // namespace Salt2D::Render

#endif // RENDER_DRAWERS_CARDDRAWER_H
