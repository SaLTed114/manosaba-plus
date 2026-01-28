// Render/Demo/CubeDemo.h
#ifndef RENDER_DEMO_CUBEDEMO_H
#define RENDER_DEMO_CUBEDEMO_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Render/Pipelines/MeshPipeline.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class ShaderManager;

class CubeDemo {
public:
    void Initialize(const RHI::DX11::DX11Device& device, ShaderManager& shaderManager);
    void Draw(const RHI::DX11::DX11Device& device, const DirectX::XMMATRIX& worldViewProj);

private:
    MeshPipeline meshPipeline_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib_;
    UINT indexCount_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DEMO_CUBEDEMO_H
