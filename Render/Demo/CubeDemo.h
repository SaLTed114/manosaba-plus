// Render/Demo/CubeDemo.h
#ifndef RENDER_DEMO_CUBEDEMO_H
#define RENDER_DEMO_CUBEDEMO_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

struct PassContext;

class CubeDemo {
public:
    void Initialize(const RHI::DX11::DX11Device& device);
    void Draw(PassContext& ctx, const DirectX::XMMATRIX& worldViewProj);

    ID3D11Buffer* GetVertexBuffer() const { return vb_.Get(); }
    ID3D11Buffer* GetIndexBuffer() const { return ib_.Get(); }
    uint32_t GetIndexCount() const { return indexCount_; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib_;
    uint32_t indexCount_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DEMO_CUBEDEMO_H
