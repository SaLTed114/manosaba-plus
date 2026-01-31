// Render/Drawers/MeshDrawer.h
#ifndef RENDER_DRAWERS_MESHDRAWER_H
#define RENDER_DRAWERS_MESHDRAWER_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

struct PassContext;

class MeshDrawer {
public:
    void Initialize(const RHI::DX11::DX11Device& device);

    void DrawMesh(
        PassContext& ctx,
        ID3D11Buffer* vertexBuffer,
        ID3D11Buffer* indexBuffer,
        uint32_t indexCount,
        const DirectX::XMMATRIX& worldViewProj
    );
};

} // namespace Salt2D::Render

#endif // RENDER_DRAWERS_MESHDRAWER_H
