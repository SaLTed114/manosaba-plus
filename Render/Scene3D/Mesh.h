// Render/Scene3D/Mesh.h
#ifndef RENDER_SCENE3D_MESH_H
#define RENDER_SCENE3D_MESH_H

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

namespace Salt2D::Render::Scene3D {

struct Mesh {
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb;
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib;
    uint32_t stride = 0;
    DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
    uint32_t indexCount = 0;
};

} // namespace Salt2D::Render::Scene3D

#endif // RENDER_SCENE3D_MESH_H
