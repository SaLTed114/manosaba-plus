// RHI/DX11/DX11DepthBuffer.h
#ifndef RHI_DX11_DX11DEPTHBUFFER_H
#define RHI_DX11_DX11DEPTHBUFFER_H

#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::RHI::DX11 {

class DX11Device;

class DX11DepthBuffer {
public:
    DX11DepthBuffer() = default;

    static DX11DepthBuffer Create(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);

    void Resize(
        const DX11Device& device,
        uint32_t width,
        uint32_t height);

    void Clear(
        ID3D11DeviceContext* context,
        float depth = 1.0f,
        uint8_t stencil = 0) const;

    uint32_t GetWidth()  const { return width_; }
    uint32_t GetHeight() const { return height_; }
    DXGI_FORMAT GetFormat() const { return format_; }

    ID3D11DepthStencilView* DSV() const { return dsv_.Get(); }

private:
    void CreateResources(
        const DX11Device& device,
        uint32_t width,
        uint32_t height,
        DXGI_FORMAT format);

private:
    uint32_t width_  = 0;
    uint32_t height_ = 0;
    DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;

    Microsoft::WRL::ComPtr<ID3D11Texture2D>          texture_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   dsv_;

};

} // namespace Salt2D::RHI::DX11

#endif // RHI_DX11_DX11DEPTHBUFFER_H
