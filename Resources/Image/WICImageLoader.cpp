// Resources/Image/WICImageLoader.cpp
#include "WICImageLoader.h"
#include <wincodec.h>
#include <wrl.h>
#include <combaseapi.h>
#include <stdexcept>

using Microsoft::WRL::ComPtr;

namespace Salt2D::Resources {

namespace {

struct ComInitOnce {
    ComInitOnce() {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
            throw std::runtime_error("Failed to initialize COM library.");
        }
    }
};

static IWICImagingFactory* GetWICFactory() {
    static ComInitOnce comInitOnce;
    static ComPtr<IWICImagingFactory> wicFactory;
    if (!wicFactory) {
        if (FAILED(CoCreateInstance(
                CLSID_WICImagingFactory, nullptr,
                CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)))) {
            throw std::runtime_error("Failed to create WIC Imaging Factory.");
        }
    }
    return wicFactory.Get();
}

} // Anonymous namespace

bool LoadImageRGBA8_WIC(const std::filesystem::path& filepath, ImageRGBA8& outImage) {
    auto* wicFactory = GetWICFactory();

    ComPtr<IWICBitmapDecoder> decoder;
    if (FAILED(wicFactory->CreateDecoderFromFilename(
            filepath.c_str(), nullptr, GENERIC_READ,
            WICDecodeMetadataCacheOnLoad, &decoder))) {
        return false;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    if (FAILED(decoder->GetFrame(0, &frame))) return false;

    UINT width = 0, height = 0;
    if (FAILED(frame->GetSize(&width, &height))) return false;

    ComPtr<IWICFormatConverter> converter;
    if (FAILED(wicFactory->CreateFormatConverter(&converter))) return false;

    if (FAILED(converter->Initialize(
            frame.Get(), GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone, nullptr, 0.0,
            WICBitmapPaletteTypeCustom))) {
        return false;
    }

    outImage.width  = static_cast<uint32_t>(width);
    outImage.height = static_cast<uint32_t>(height);
    outImage.rowPitch = outImage.width * 4;
    outImage.pixels.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);

    const UINT stride = width * 4;
    const UINT bufferSize = stride * height;
    if (FAILED(converter->CopyPixels(
            nullptr, stride, bufferSize,
            outImage.pixels.data()))) {
        return false;
    }

    return true;
}

} // namespace Salt2D::Resources
