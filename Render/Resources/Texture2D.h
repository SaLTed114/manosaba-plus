// Render/Resources/Texture2D.h
#ifndef RENDER_RESOURCES_TEXTURE2D_H
#define RENDER_RESOURCES_TEXTURE2D_H

#include <cstdint>
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::Render {

class Texture2D {
public:
    Texture2D() = default;

    static Texture2D CreateRGBA8(
        ID3D11Device* device,
        uint32_t width,
        uint32_t height,
        const void* rgbaData,
        uint32_t rowPitchBytes
    );

    uint32_t GetWidth()  const { return width; }
    uint32_t GetHeight() const { return height; }

    Microsoft::WRL::ComPtr<ID3D11Texture2D> GetTexture() const { return texture_; }
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const { return srv_; }

private:
    uint32_t width  = 0;
    uint32_t height = 0;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;

};

} // namespace Salt2D::Render

#endif // RENDER_RESOURCES_TEXTURE2D_H
