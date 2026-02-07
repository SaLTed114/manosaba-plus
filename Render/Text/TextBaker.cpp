// Render/Text/TextBaker.cpp
#include "TextBaker.h"
#include "RHI/DX11/DX11Common.h"

#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>

using Microsoft::WRL::ComPtr;

namespace Salt2D::Render::Text {

void TextBaker::Initialize() {
    ThrowIfFailed(DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(dwriteFactory_.GetAddressOf())),
        "TextBaker::Initialize: DWriteCreateFactory failed.");

    ThrowIfFailed(D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory_.GetAddressOf()),
        "TextBaker::Initialize: D2D1CreateFactory failed.");

    ThrowIfFailed(CoCreateInstance(
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(wicFactory_.GetAddressOf())),
        "TextBaker::Initialize: CoCreateInstance for WICImagingFactory failed.");
}

BakedText TextBaker::BakeToTexture(
    const RHI::DX11::DX11Device& device,
    const std::wstring& text,
    const TextStyle& style,
    float layoutW,
    float layoutH
)  {
    if (!dwriteFactory_ || !d2dFactory_ || !wicFactory_) {
        throw std::runtime_error("TextBaker::BakeText: TextBaker is not initialized.");
    }

    if (text.empty()) {
        BakedText emptyResult;
        emptyResult.tex = RHI::DX11::DX11Texture2D::CreateDynamicSRV(
            device, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM);

        uint8_t clearPixel[4] = { 255, 255, 255, 0 };
        emptyResult.tex.UpdateDynamic(device.GetContext(), clearPixel, 4);
        emptyResult.w = 1; emptyResult.h = 1;
        return emptyResult;
    }

    ComPtr<IDWriteTextFormat> textFormat;
    ThrowIfFailed(dwriteFactory_->CreateTextFormat(
        style.fontFamily.c_str(), nullptr,
        style.weight, style.style, style.stretch,
        style.fontSize,
        L"zh-cn",
        textFormat.GetAddressOf()),
        "TextBaker::BakeText: CreateTextFormat failed.");

    textFormat->SetTextAlignment(style.textAlign);
    textFormat->SetParagraphAlignment(style.paraAlign);
    textFormat->SetWordWrapping(style.wrapping);

    if (!std::isfinite(layoutW) || layoutW <= 0.0f || 
        !std::isfinite(layoutH) || layoutH <= 0.0f) {
        throw std::runtime_error("TextBaker::BakeText: Invalid layout dimensions - layoutW=" + 
            std::to_string(layoutW) + ", layoutH=" + std::to_string(layoutH));
    }

    ComPtr<IDWriteTextLayout> textLayout;
    ThrowIfFailed(dwriteFactory_->CreateTextLayout(
        text.c_str(), static_cast<UINT32>(text.length()),
        textFormat.Get(), layoutW, layoutH,
        textLayout.GetAddressOf()),
        "TextBaker::BakeText: CreateTextLayout failed.");

    DWRITE_TEXT_METRICS textMetrics;
    ThrowIfFailed(textLayout->GetMetrics(&textMetrics),
        "TextBaker::BakeText: GetMetrics failed.");

    uint32_t texW = static_cast<uint32_t>(std::ceil(textMetrics.widthIncludingTrailingWhitespace)) + 2;
    uint32_t texH = static_cast<uint32_t>(std::ceil(textMetrics.height)) + 2;
    texW = (std::max)(texW, 1u);
    texH = (std::max)(texH, 1u);

    ComPtr<IWICBitmap> wicBitmap;
    ThrowIfFailed(wicFactory_->CreateBitmap(
        texW, texH, GUID_WICPixelFormat32bppPBGRA,
        WICBitmapCacheOnLoad, wicBitmap.GetAddressOf()),
        "TextBaker::BakeText: CreateBitmap failed.");

    D2D1_RENDER_TARGET_PROPERTIES rtProps{};
    rtProps.type = D2D1_RENDER_TARGET_TYPE_SOFTWARE;
    rtProps.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    rtProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    rtProps.dpiX = 96.0f;
    rtProps.dpiY = 96.0f;
    rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;
    rtProps.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

    ComPtr<ID2D1RenderTarget> d2dRenderTarget;
    ThrowIfFailed(d2dFactory_->CreateWicBitmapRenderTarget(
        wicBitmap.Get(), rtProps, d2dRenderTarget.GetAddressOf()),
        "TextBaker::BakeText: CreateWicBitmapRenderTarget failed.");

    d2dRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    ComPtr<ID2D1SolidColorBrush> brush;
    ThrowIfFailed(d2dRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF{1,1,1,1}, brush.GetAddressOf()),
        "TextBaker::BakeText: CreateSolidColorBrush failed.");

    d2dRenderTarget->BeginDraw();
    d2dRenderTarget->Clear(D2D1::ColorF{0,0,0,0});

    D2D1_POINT_2F origin{1.0f, 1.0f};
    d2dRenderTarget->DrawTextLayout(origin, textLayout.Get(), brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);

    HRESULT hr = d2dRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET) {
        // placeholder for handling target recreation if needed in future
        throw std::runtime_error("TextBaker::BakeText: D2D render target needs to be recreated.");
    }
    ThrowIfFailed(hr, "TextBaker::BakeText: EndDraw failed.");

    ComPtr<IWICBitmapLock> bitmapLock;
    WICRect lockRect = { 0, 0, static_cast<INT>(texW), static_cast<INT>(texH) };
    ThrowIfFailed(wicBitmap->Lock(&lockRect, WICBitmapLockRead, bitmapLock.GetAddressOf()),
        "TextBaker::BakeText: Bitmap Lock failed.");

    UINT stride = 0;
    UINT bufferSize = 0;
    BYTE* bitmapData = nullptr;
    ThrowIfFailed(bitmapLock->GetStride(&stride),
        "TextBaker::BakeText: GetStride failed.");
    ThrowIfFailed(bitmapLock->GetDataPointer(&bufferSize, &bitmapData),
        "TextBaker::BakeText: GetDataPointer failed.");

    const uint32_t expectedRowPitch = texW * 4;
    if (stride != expectedRowPitch) {
        throw std::runtime_error("TextBaker::BakeText: Unexpected stride in WIC bitmap data.");
    }

    std::vector<uint8_t> rgbaData(texH * expectedRowPitch);

    for (uint32_t row = 0; row < texH; ++row) {
        const uint8_t* src = bitmapData + row * stride;
        uint8_t* dst = rgbaData.data() + row * expectedRowPitch;

        for (uint32_t col = 0; col < texW; ++col) {
            const uint8_t b = src[col * 4 + 0];
            const uint8_t g = src[col * 4 + 1];
            const uint8_t r = src[col * 4 + 2];
            const uint8_t a = src[col * 4 + 3];

            uint8_t cov = (std::max)({a, r, g, b});

            dst[col * 4 + 0] = 255;
            dst[col * 4 + 1] = 255;
            dst[col * 4 + 2] = 255;
            dst[col * 4 + 3] = cov;
        }
    }

    BakedText result;
    result.tex = RHI::DX11::DX11Texture2D::CreateDynamicSRV(
        device, texW, texH, DXGI_FORMAT_R8G8B8A8_UNORM);
    result.tex.UpdateDynamic(device.GetContext(), rgbaData.data(), expectedRowPitch);
    result.w = texW;
    result.h = texH;

    return result;
}

} // namespace Salt2D::Render::Text
