// Game/Director/StageCameraDirector.h
#ifndef GAME_DIRECTOR_STAGECAMERADIRECTOR_H
#define GAME_DIRECTOR_STAGECAMERADIRECTOR_H

#include "StageWorld.h"
#include "Game/Story/StoryTables.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/Director/Filters/LowPass.h"
#include "Render/Scene3D/Camera3D.h"

namespace Salt2D::Game::Director {

struct CameraDirectorConfig {
    float dist = 2.0f;
    float liftY = 0.15f;

    // lowpass filter for camera movement to reduce jitter
    float tauEyeSec = 0.60f;
    float tauTargetSec = 0.50f;
    float tauFovSec = 0.18f;
    
    float snapDist = 3.0f;
    float snapAngleY = 0.0f;
};

struct StageCameraSample {
    DirectX::XMFLOAT3 eye{};
    DirectX::XMFLOAT3 target{};
    float fovYRad = DirectX::XM_PIDIV4; // 45 degrees
};

class StageCameraDirector {
public:
    void Initialize(StageWorld* world,
        const Story::StoryTables* tables,
        Render::Scene3D::Camera3D* camera);

    void SetConfig(const CameraDirectorConfig& cfg) { cfg_ = cfg; }

    void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft);

private:
    bool EvalStageCamera(const Story::PerformanceDef& perf,
        const Anchor& anchor, float u01, StageCameraSample& out) const;
    
    void DefaultCamera(const Anchor& anchor, StageCameraSample& out) const;

private:
    CameraDirectorConfig cfg_{};

    StageWorld* world_ = nullptr;
    const Story::StoryTables* tables_ = nullptr;
    Render::Scene3D::Camera3D* camera_ = nullptr;

    StageCameraSample lastSample_{};

private:
    LowPassVec3 eyeLP_;
    LowPassVec3 targetLP_;
    LowPass1 fovLP_;

    void InvalidateLP();
    void ApplyLowPass(float dt, StageCameraSample& sample);
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_STAGECAMERADIRECTOR_H
