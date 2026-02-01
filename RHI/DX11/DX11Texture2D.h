// RHI/DX11/DX11Texture2D.h
#ifndef RHI_DX11_DX11TEXTURE2D_H
#define RHI_DX11_DX11TEXTURE2D_H

#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::RHI::DX11 {

class DX11Device;

class DX11Texture2D {
public:
    DX11Texture2D() = default;

    static DX11Texture2D CreateRGBA8(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        const void* rgbaData,
        uint32_t rowPitchBytes);

    static DX11Texture2D CreateRenderTarget(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

    static DX11Texture2D CreateImmutableSRV(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format,
        const void* initData,
        uint32_t rowPitchBytes);

    static DX11Texture2D CreateDynamicSRV(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

    void UpdateDynamic(
        ID3D11DeviceContext* context,
        const void* rgbaData,
        uint32_t rowPitchBytes);

    uint32_t GetWidth()  const { return width_; }
    uint32_t GetHeight() const { return height_; }
    DXGI_FORMAT GetFormat() const { return format_; }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const { return texture_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return srv_; }
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   GetRTV() const { return rtv_; }

    ID3D11ShaderResourceView* SRV() const { return srv_.Get(); }
    ID3D11RenderTargetView*   RTV() const { return rtv_.Get(); }

private:
    uint32_t width_  = 0;
    uint32_t height_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;

    D3D11_USAGE usage_ = D3D11_USAGE_DEFAULT;
    UINT cpuAccessFlags_ = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   rtv_;
};

} // namespace Salt2D::RHI::DX11

#endif // RHI_DX11_DX11TEXTURE2D_H
