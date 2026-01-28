// Rebder/DEMO/TriangleDemo.h
#ifndef RENDER_DEMO_TRIANGLEDEMO_H
#define RENDER_DEMO_TRIANGLEDEMO_H

#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::Render {

class ShaderManager;

class TriangleDemo {
public:
    void Initialize(ID3D11Device* device, ShaderManager& shaderManager);
    void Draw(ID3D11DeviceContext* ctx, ID3D11RenderTargetView* rtv);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       vb_;

    UINT stride_ = 0;
    UINT offset_ = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_DEMO_TRIANGLEDEMO_H
