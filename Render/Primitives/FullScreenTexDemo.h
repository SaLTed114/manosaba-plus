// Render/Primitives/FullScreenTexDemo.h
#ifndef RENDER_PRIMITIVES_FULLSCREENTEXDEMO_H
#define RENDER_PRIMITIVES_FULLSCREENTEXDEMO_H

#include <wrl/client.h>
#include <d3d11.h>
#include "Render/Resources/Texture2D.h"

namespace Salt2D::Render {

class ShaderManager;

class FullScreenTexDemo {
public:
    void Initialize(ID3D11Device* device, ShaderManager& shaderMgr);
    void Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  ps;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samp_;
    
    Texture2D texture_;
};

} // namespace Salt2D::Render

#endif // RENDER_PRIMITIVES_FULLSCREENTEXDEMO_H
