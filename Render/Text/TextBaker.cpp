// Render/Text/TextBaker.cpp
#include "TextBaker.h"
#include "RHI/DX11/DX11Common.h"

#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <cstring>

using Microsoft::WRL::ComPtr;

namespace Salt2D::Render::Text {

namespace {

static void BuildLineRectsFromLayout(
    IDWriteTextLayout* layout,
    int pad, uint32_t texW, uint32_t texH,
    std::vector<Render::RectF>& outRects
) {
    outRects.clear();
    if (!layout) return;

    UINT32 lineCount = 0;
    HRESULT hr = layout->GetLineMetrics(nullptr, 0, &lineCount);
    if (hr != E_NOT_SUFFICIENT_BUFFER && FAILED(hr)) return;
    if (lineCount == 0) return;

    std::vector<DWRITE_LINE_METRICS> lineMetrics(lineCount);
    ThrowIfFailed(layout->GetLineMetrics(lineMetrics.data(), lineCount, &lineCount),
        "BuildLineRectsFromLayout: GetLineMetrics failed.");

    outRects.reserve(lineCount);

    UINT32 textPos = 0;
    for (UINT32 i = 0; i < lineCount; ++i) {
        const auto& lm = lineMetrics[i];

        UINT32 lineLength = lm.length;
        if (lm.newlineLength <= lineLength) lineLength -= lm.newlineLength;

        FLOAT caretX = 0.0f, caretY = 0.0f;
        DWRITE_HIT_TEST_METRICS caretMetrics{};
        ThrowIfFailed(layout->HitTestTextPosition(textPos, false, &caretX, &caretY, &caretMetrics),
            "BuildLineRectsFromLayout: HitTestTextPosition failed.");

        Render::RectF lineRect{ caretMetrics.left, caretMetrics.top, 0.0f, caretMetrics.height };

        if (lineLength > 0) {
            UINT32 hitCount = 0;
            HRESULT hr2 = layout->HitTestTextRange(textPos, lineLength, 0.0f, 0.0f, nullptr, 0, &hitCount);
            if (hr2 != E_NOT_SUFFICIENT_BUFFER && hr2 != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) && FAILED(hr2)) {
                throw std::runtime_error("BuildLineRectsFromLayout: HitTestTextRange failed.");
            }

            if (hitCount > 0) {
                std::vector<DWRITE_HIT_TEST_METRICS> hitMetrics(hitCount);
                UINT32 actualHitCount = 0;

                HRESULT hr3 = layout->HitTestTextRange(textPos, lineLength, 0.0f, 0.0f,
                    hitMetrics.data(), hitCount, &actualHitCount);

                if (hr3 != E_NOT_SUFFICIENT_BUFFER && hr3 != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER) && FAILED(hr3)) {
                    ThrowIfFailed(hr3, "BuildLineRectsFromLayout: HitTestTextRange (metrics) failed.");
                }

                float x0 = hitMetrics[0].left;
                float y0 = hitMetrics[0].top;
                float x1 = hitMetrics[0].left + hitMetrics[0].width;
                float y1 = hitMetrics[0].top  + hitMetrics[0].height;
                for (UINT32 j = 1; j < actualHitCount; ++j) {
                    const auto& hm = hitMetrics[j];
                    x0 = (std::min)(x0, hm.left);
                    y0 = (std::min)(y0, hm.top);
                    x1 = (std::max)(x1, hm.left + hm.width);
                    y1 = (std::max)(y1, hm.top  + hm.height);
                }
                lineRect = Render::RectF{x0, y0, x1 - x0, y1 - y0};
            }
        }

        lineRect.x += static_cast<float>(pad);
        lineRect.y += static_cast<float>(pad);

        lineRect.x = (std::max)(lineRect.x, 0.0f);
        lineRect.y = (std::max)(lineRect.y, 0.0f);
        if (lineRect.x + lineRect.w > static_cast<float>(texW)) lineRect.w = (std::max)(static_cast<float>(texW) - lineRect.x, 0.0f);
        if (lineRect.y + lineRect.h > static_cast<float>(texH)) lineRect.h = (std::max)(static_cast<float>(texH) - lineRect.y, 0.0f);

        outRects.push_back(lineRect);
        textPos += lineMetrics[i].length;
    }
}

} // Anonymous namespace

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

    if (style.lineHeightScale > 0.0f) {
        const float lineSpacing = style.lineHeightScale * style.fontSize;
        const float baseline    = lineSpacing * style.baselineScale;
        ThrowIfFailed(textLayout->SetLineSpacing(
            DWRITE_LINE_SPACING_METHOD_UNIFORM, lineSpacing, baseline),
            "TextBaker::BakeText: SetLineSpacing failed.");
    }

    DWRITE_TEXT_METRICS textMetrics;
    ThrowIfFailed(textLayout->GetMetrics(&textMetrics),
        "TextBaker::BakeText: GetMetrics failed.");

    const int radius = (style.outlinePx > 0.0f) ? static_cast<int>(std::ceil(style.outlinePx)) : 0;
    const int basePad   = 1;
    const int filterPad = 1;
    const int pad = basePad + filterPad + radius;

    uint32_t texW = static_cast<uint32_t>(std::ceil(textMetrics.widthIncludingTrailingWhitespace)) + 2 * pad;
    uint32_t texH = static_cast<uint32_t>(std::ceil(textMetrics.height)) + 2 * pad;
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

    D2D1_POINT_2F origin{static_cast<float>(pad), static_cast<float>(pad)};

    if (radius > 0) {
        brush->SetColor(D2D1::ColorF{0,0,0,1});
        const int r2 = radius * radius;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                if (dx == 0 && dy == 0) continue;
                if (dx*dx + dy*dy > r2) continue;
                D2D1_POINT_2F offset{static_cast<float>(pad + dx), static_cast<float>(pad + dy)};
                d2dRenderTarget->DrawTextLayout(offset,
                    textLayout.Get(), brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
            }
        }
    }

    brush->SetColor(D2D1::ColorF{1,1,1,1});
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

            uint8_t outR = 0, outG = 0, outB = 0;
            if (a != 0) {
                outR = static_cast<uint8_t>((std::min)(255u, static_cast<uint32_t>(r) * 255u / static_cast<uint32_t>(a)));
                outG = static_cast<uint8_t>((std::min)(255u, static_cast<uint32_t>(g) * 255u / static_cast<uint32_t>(a)));
                outB = static_cast<uint8_t>((std::min)(255u, static_cast<uint32_t>(b) * 255u / static_cast<uint32_t>(a)));
            }

            dst[col * 4 + 0] = outR;
            dst[col * 4 + 1] = outG;
            dst[col * 4 + 2] = outB;
            dst[col * 4 + 3] = a;
        }
    }

    BakedText result;
    result.tex = RHI::DX11::DX11Texture2D::CreateDynamicSRV(
        device, texW, texH, DXGI_FORMAT_R8G8B8A8_UNORM);
    result.tex.UpdateDynamic(device.GetContext(), rgbaData.data(), expectedRowPitch);
    result.w = texW;
    result.h = texH;

    result.lineRectsPx.clear();
    BuildLineRectsFromLayout(textLayout.Get(), pad, texW, texH, result.lineRectsPx);

    return result;
}

} // namespace Salt2D::Render::Text
