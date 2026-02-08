// Render/Text/TextBaker.h
#ifndef RENDER_TEXT_TEXTBAKER_H
#define RENDER_TEXT_TEXTBAKER_H

#include <string>
#include <vector>
#include <wrl/client.h>
#include <d3d11.h>
#include <dwrite.h>
#include <d2d1.h>
#include <wincodec.h>

#include "RHI/DX11/DX11Texture2D.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Render::Text {

struct TextStyle {
    std::wstring fontFamily = L"SimSun";
    float fontSize = 28.0f;
    DWRITE_FONT_WEIGHT  weight  = DWRITE_FONT_WEIGHT_REGULAR;
    DWRITE_FONT_STYLE   style   = DWRITE_FONT_STYLE_NORMAL;
    DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;

    DWRITE_TEXT_ALIGNMENT      textAlign = DWRITE_TEXT_ALIGNMENT_LEADING;
    DWRITE_PARAGRAPH_ALIGNMENT paraAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
    DWRITE_WORD_WRAPPING       wrapping  = DWRITE_WORD_WRAPPING_WRAP;

    float lineHeightScale = 0.0f;
    float baselineScale = 1.0f;
};

struct BakedText {
    RHI::DX11::DX11Texture2D tex;
    uint32_t w = 0;
    uint32_t h = 0;
};

class TextBaker {
public:
    void Initialize();
    BakedText BakeToTexture(const RHI::DX11::DX11Device& device,
        const std::wstring& text, const TextStyle& style,
        float layoutW, float layoutH);

private:
    Microsoft::WRL::ComPtr<IDWriteFactory>       dwriteFactory_;
    Microsoft::WRL::ComPtr<ID2D1Factory>         d2dFactory_;
    Microsoft::WRL::ComPtr<IWICImagingFactory>   wicFactory_;

};

} // namespace Salt2D::Render::Text

#endif // RENDER_TEXT_TEXTBAKER_H
