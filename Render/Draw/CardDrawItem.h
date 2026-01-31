// Render/Draw/CardDrawItem.h
#ifndef RENDER_DRAW_CARDDRAWITEM_H
#define RENDER_DRAW_CARDDRAWITEM_H

#include <DirectXMath.h>
#include <d3d11.h>

namespace Salt2D::Render {

struct CardDrawItem {
    ID3D11ShaderResourceView* srv = nullptr;
    DirectX::XMFLOAT3 pos{0.0f, 0.0f, 0.0f};
    float yaw = 0.0f;
    DirectX::XMFLOAT2 size{1.0f, 1.6f};
    float alphaCut = 0.5f;
};

} // namespace Salt2D::Render

#endif // RENDER_DRAW_CARDDRAWITEM_H
