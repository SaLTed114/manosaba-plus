// Game/Director/StageCamera/StageCameraTypes.h
#ifndef GAME_DIRECTOR_STAGECAMERA_STAGECAMERATYPES_H
#define GAME_DIRECTOR_STAGECAMERA_STAGECAMERATYPES_H

#include <DirectXMath.h>

namespace Salt2D::Game::Director {

struct StageCameraSample {
    DirectX::XMFLOAT3 eye{};
    DirectX::XMFLOAT3 target{};
    float fovYRad = DirectX::XM_PIDIV4; // 45 degrees
};

struct StageCameraRotationResult {
    bool useFixed = false;
    DirectX::XMFLOAT4 quat{0,0,0,1}; // identity
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_STAGECAMERA_STAGECAMERATYPES_H
