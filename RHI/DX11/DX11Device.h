// RHI/DX11/DX11Device.h
#ifndef RHI_DX11_DEVICE_H
#define RHI_DX11_DEVICE_H

#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi1_6.h>

namespace Salt2D::RHI::DX11 {

class DX11Device {
public:
    explicit DX11Device(const Microsoft::WRL::ComPtr<IDXGIAdapter1>& adapter = nullptr);

    ID3D11Device*        GetDevice()  const { return device_.Get(); }
    ID3D11DeviceContext* GetContext() const { return context_.Get(); }

    Microsoft::WRL::ComPtr<IDXGIAdapter1> GetAdapter() const { return adapter_; }
    Microsoft::WRL::ComPtr<IDXGIFactory2> GetFactory() const { return factory_; }
    D3D_FEATURE_LEVEL GetFeatureLevel() const { return featureLevel_; }

    bool IsDebugLayerEnabled() const { return debugLayerEnabled_; }

private:
    Microsoft::WRL::ComPtr<ID3D11Device>        device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter_;
    Microsoft::WRL::ComPtr<IDXGIFactory2> factory_;

    D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_0;
    bool debugLayerEnabled_ = false;
};

} // namespace Salt2D::RHI::DX11

#endif // RHI_DX11_DEVICE_H
