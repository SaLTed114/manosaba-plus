// Game/Director/StageCameraDirector.cpp
#include "StageCameraDirector.h"
#include "Utils/MathUtils.h"

#include <DirectXMath.h>
#include <iostream>

using namespace DirectX;

namespace Salt2D::Game::Director {

// =========================== Begin of helpers ===========================

static inline float EvalU01(float cur, float total) {
    if (total <= 0) return 1.0f;
    return Utils::Clamp01(cur / total);
}

// =========================== End of helpers ===========================

void StageCameraDirector::Initialize(
    StageWorld* world,
    const Story::StoryTables* tables,
    Render::Scene3D::Camera3D* camera
) {
    world_ = world;
    tables_ = tables;
    camera_ = camera;

    vnFade_.SetDuration(0.5f);
    vnFade_.Reset();
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

        const Story::PerformanceDef* perf = nullptr;
        if (!view->perfId.empty()) perf = tables_->perf.Find(view->perfId);

        StageCameraSample sample{};
        StageCameraRotationResult rotation{};

        vnFade_.UpdateVN(player.CurrentNodeId(),
            view->speaker, view->lineSerial,
            view->lineDone, static_cast<float>(ft.dtSec));

        sceneCrossfade_ = vnFade_.CrossfadeT();
        lockPrevScene_  = vnFade_.LockPrev();

        const float cur   = view->revealCpF;
        const float total = static_cast<float>(view->totalCp);
        const float u = EvalU01(cur, total);

        bool ok = false;
        if (perf && perf->stage.cameraTrack.has_value()) {
            const auto& track = perf->stage.cameraTrack.value();
            ok = StageCameraSolver::EvalAnchorTrackLinear(track, anchor, u, sample);
            if (ok) rotation = StageCameraSolver::EvalRotation(track.rotation, anchor, sample);
        }
        if (!ok) {
            StageCameraSolver::DefaultCamera(anchor, sample);
            rotation.useFixed = false;
        }

        camera_->SetPosition(sample.eye);
        camera_->SetFovY(sample.fovYRad);
                    
        if (rotation.useFixed) {
            camera_->SetRotationQuat(rotation.quat);
        } else {
            camera_->LookAt(sample.target);
        }

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
        if (!view->perfId.empty()) perf = tables_->perf.Find(view->perfId);

        StageCameraSample sample{};
        StageCameraRotationResult rotation{};

        const float curSec   = view->stmtTotalSec - view->stmtRemainSec;
        const float totalSec = view->stmtTotalSec;
        const float u = EvalU01(curSec, totalSec);

        bool ok = false;
        if (perf && perf->stage.cameraTrack.has_value()) {
            const auto& track = perf->stage.cameraTrack.value();
            ok = StageCameraSolver::EvalAnchorTrackLinear(track, anchor, u, sample);
            if (ok) rotation = StageCameraSolver::EvalRotation(track.rotation, anchor, sample);
        }
        if (!ok) {
            StageCameraSolver::DefaultCamera(anchor, sample);
            rotation.useFixed = false;
        }

        camera_->SetPosition(sample.eye);
        camera_->SetFovY(sample.fovYRad);
        
        if (rotation.useFixed) {
            camera_->SetRotationQuat(rotation.quat);
        } else {
            camera_->LookAt(sample.target);
        }

        break;
    }
    default:
        break;
    }
}

} // namespace Salt2D::Game::Director
