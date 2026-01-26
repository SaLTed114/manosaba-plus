// Render/DX11CommonState.h
#ifndef RENDER_DX11COMMONSTATE_H
#define RENDER_DX11COMMONSTATE_H

#include <wrl/client.h>
#include <d3d11.h>

namespace Salt2D::Render {

struct DX11CommonState {
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthOff;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthRW;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthRO;

    Microsoft::WRL::ComPtr<ID3D11BlendState> blendOff;
    Microsoft::WRL::ComPtr<ID3D11BlendState> blendAlpha;

    void Initialize(ID3D11Device* device);
};

struct DX11StateCache {
    ID3D11DepthStencilState* currentDepthStencilState = nullptr;
    UINT                     currentStencilRef        = 0;

    ID3D11BlendState*        currentBlendState        = nullptr;
    UINT                     currentSampleMask        = 0xFFFFFFFF;
    FLOAT                    currentBlendFactor[4]    = {0.0f, 0.0f, 0.0f, 0.0f};

    void SetDepthStencilState(
        ID3D11DeviceContext* ctx,
        ID3D11DepthStencilState* dss,
        UINT stencilRef = 0
    ) {
        if (currentDepthStencilState != dss || currentStencilRef != stencilRef) {
            ctx->OMSetDepthStencilState(dss, stencilRef);
            currentDepthStencilState = dss;
            currentStencilRef = stencilRef;
        }
    }

    void SetBlendState(
        ID3D11DeviceContext* ctx,
        ID3D11BlendState* bs,
        const FLOAT blendFactor[4] = nullptr,
        UINT sampleMask = 0xFFFFFFFF
    ) {
        float bf[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        if (blendFactor) {
            for (int i = 0; i < 4; ++i) bf[i] = blendFactor[i];
        }

        bool sameFactor = (
            bf[0] == currentBlendFactor[0] && bf[1] == currentBlendFactor[1] &&
            bf[2] == currentBlendFactor[2] && bf[3] == currentBlendFactor[3]);

        if (currentBlendState != bs || !sameFactor || currentSampleMask != sampleMask) {
            ctx->OMSetBlendState(bs, bf, sampleMask);
            currentBlendState = bs;
            currentSampleMask = sampleMask;
            for (int i = 0; i < 4; ++i) currentBlendFactor[i] = bf[i];
        }
    }

    void Reset() {
        currentDepthStencilState = nullptr;
        currentStencilRef = 0;

        currentBlendState = nullptr;
        currentSampleMask = 0xFFFFFFFF;
        for (auto& v : currentBlendFactor) v = 0.0f;
    }
};

} // namespace Salt2D::Render

#endif // RENDER_DX11COMMONSTATE_H
