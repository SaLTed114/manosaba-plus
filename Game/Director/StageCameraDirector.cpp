// Game/Director/StageCameraDirector.cpp
#include "StageCameraDirector.h"
#include "Utils/MathUtils.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace Salt2D::Game::Director {

// =========================== Begin of helpers ===========================

static inline XMFLOAT3 NormalizeXZ(const XMFLOAT3& v) {
    float len = std::sqrt(v.x * v.x + v.z * v.z);
    if (len < 1e-6f) return {0, 0, 1};
    return {v.x / len, 0, v.z / len};
}

static inline XMFLOAT3 ComputeSide(const XMFLOAT3& forward) {
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMVECTOR r  = XMLoadFloat3(&forward);
    XMVECTOR s  = XMVector3Cross(r, up);
    XMFLOAT3 side;
    XMStoreFloat3(&side, s);
    return NormalizeXZ(side);
}

static inline float StatementU01(float totalSec, float remainSec) {
    if (totalSec <= 0) return 1.0f;
    return Utils::Clamp01(1.0f - remainSec / totalSec);
}

static inline XMFLOAT3 Sub(const XMFLOAT3& a, const XMFLOAT3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline float Len(const XMFLOAT3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// =========================== End of helpers ===========================


// =========================== Begin of evaluation ===========================

bool StageCameraDirector::EvalStageCamera(const Story::PerformanceDef& perf,
    const Anchor& anchor, float u01, StageCameraSample& out
) const {
    if (!perf.stage.cameraTrack.has_value()) return false;
    const auto& track = perf.stage.cameraTrack.value();

    if (track.space != Story::CameraSpace::Anchor) return false; // unsupported space

    const float t = Utils::Clamp01(u01);

    const float dist   = Utils::Lerp(track.start.dist,    track.end.dist,    t);
    const float liftY  = Utils::Lerp(track.start.liftY,   track.end.liftY,   t);
    const float sideX  = Utils::Lerp(track.start.sideX,   track.end.sideX,   t);
    const float fovDeg = Utils::Lerp(track.start.fovYDeg, track.end.fovYDeg, t);

    const XMFLOAT3 up{0, 1, 0};
    const XMFLOAT3 face = anchor.face;
    const XMFLOAT3 side = ComputeSide(face);

    out.eye = {
        anchor.head.x + face.x * dist + up.x * liftY + side.x * sideX,
        anchor.head.y + up.y * liftY,
        anchor.head.z + face.z * dist + up.z * liftY + side.z * sideX
    };

    out.target = {
        anchor.head.x,
        anchor.head.y + track.targetListY,
        anchor.head.z,
    };

    out.fovYRad = Utils::DegToRad(fovDeg);
    return true;
}

void StageCameraDirector::DefaultCamera(const Anchor& anchor, StageCameraSample& out) const {
    const XMFLOAT3 up{0, 1, 0};
    const XMFLOAT3 face = anchor.face;

    out.eye = {
        anchor.head.x + face.x * cfg_.dist + up.x * cfg_.liftY,
        anchor.head.y + up.y * cfg_.liftY,
        anchor.head.z + face.z * cfg_.dist + up.z * cfg_.liftY
    };

    out.target = {
        anchor.head.x,
        anchor.head.y,
        anchor.head.z,
    };

    out.fovYRad = DirectX::XM_PI / 3.0f; // 60 degrees
}

// ========================== End of evaluation ===========================

// ========================== Begin of lowpass ===========================

void StageCameraDirector::InvalidateLP() {
    eyeLP_.   Invalidate();
    targetLP_.Invalidate();
    fovLP_.   Invalidate();
}

void StageCameraDirector::ApplyLowPass(float dt, StageCameraSample& sample) {
    if (dt <= 0.0f) return;

    sample.eye     = eyeLP_.   Update(dt, sample.eye);
    sample.target  = targetLP_.Update(dt, sample.target);
    sample.fovYRad = fovLP_.   Update(dt, sample.fovYRad);
}

// ========================== End of lowpass ===========================

void StageCameraDirector::Initialize(
    StageWorld* world,
    const Story::StoryTables* tables,
    Render::Scene3D::Camera3D* camera
) {
    world_ = world;
    tables_ = tables;
    camera_ = camera;

    eyeLP_.x.tau = cfg_.tauEyeSec;
    eyeLP_.y.tau = cfg_.tauEyeSec;
    eyeLP_.z.tau = cfg_.tauEyeSec;

    targetLP_.x.tau = cfg_.tauTargetSec;
    targetLP_.y.tau = cfg_.tauTargetSec;
    targetLP_.z.tau = cfg_.tauTargetSec;

    fovLP_.tau = cfg_.tauFovSec;
}

void StageCameraDirector::Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft) {
    if (!world_ || !tables_ || !camera_) return;

    const auto& node = player.CurrentNode();
    switch (node.type) {
    case Story::NodeType::VN:
    case Story::NodeType::Error:
    case Story::NodeType::BE: {
        const auto& view = player.View().vn;
        if (!view.has_value()) break;

        const auto& speaker = view->speaker;
        const auto* cast = tables_->cast.FindByName(speaker);
        if (!cast) break;

        Anchor anchor{};
        if (!world_->FindAnchor(cast->id, anchor)) break;

        StageCameraSample sample{};
        DefaultCamera(anchor, sample);

        camera_->SetPosition(sample.eye);
        camera_->LookAt(sample.target);

        InvalidateLP();

        break;
    }
    case Story::NodeType::Debate: {
        const auto& view = player.View().debate;
        if (!view.has_value()) break;

        const auto& speaker = view->speaker;
        const auto* cast = tables_->cast.FindByName(speaker);
        if (!cast) break;

        Anchor anchor{};
        if (!world_->FindAnchor(cast->id, anchor)) break;

        const Story::PerformanceDef* perf = nullptr;
        if (!view->prefId.empty()) perf = tables_->perf.Find(view->prefId);

        StageCameraSample sample{};
        const float u = StatementU01(view->stmtTotalSec, view->stmtRemainSec);

        bool ok = false;
        if (perf) ok = EvalStageCamera(*perf, anchor, u, sample);
        if (!ok) DefaultCamera(anchor, sample);

        if (!view->menuOpen) {
            ApplyLowPass(static_cast<float>(ft.dtSec), sample);
            lastSample_ = sample;
        } else {
            // snap to last sample when menu is open
            sample = lastSample_;
        }

        camera_->SetPosition(sample.eye);
        camera_->SetFovY(sample.fovYRad);
        camera_->LookAt(sample.target);

        break;
    }
    default:
        break;
    }
}

} // namespace Salt2D::Game::Director
