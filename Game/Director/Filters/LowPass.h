// Game/Director/Filters/LowPass.h
#ifndef GAME_DIRECTOR_FILTERS_LOWPASS_H
#define GAME_DIRECTOR_FILTERS_LOWPASS_H

#include <cmath>
#include <DirectXMath.h>

namespace Salt2D::Game::Director {

struct LowPass1 {
    float tau = 0.1f; // time constant in seconds
    float y = 0.0f; // current output value
    bool has = false; // whether y is valid

    float alpha; // computed as 1 - exp(-dt / tau)

    void Invalidate() { has = false; }
    void Reset(float v) { y = v; has = true; }
    float Update(float dt, float x) {
        if (!has) { Reset(x); return y; }
        if (tau <= 1e-6f) { y = x; return y; }
        alpha = 1.0f - std::exp(-dt / tau);
        y += alpha * (x - y);
        return y;
    }
};

struct LowPassVec3 {
    LowPass1 x;
    LowPass1 y;
    LowPass1 z;

    void Invalidate() { x.Invalidate(); y.Invalidate(); z.Invalidate(); }
    void Reset(const DirectX::XMFLOAT3& v) { x.Reset(v.x); y.Reset(v.y); z.Reset(v.z); }

    DirectX::XMFLOAT3 Update(float dt, const DirectX::XMFLOAT3& v) {
        return { x.Update(dt, v.x), y.Update(dt, v.y), z.Update(dt, v.z) };
    }
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_FILTERS_LOWPASS_H
