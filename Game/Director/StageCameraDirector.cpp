// Game/Director/StageCameraDirector.cpp
#include "StageCameraDirector.h"
#include <DirectXMath.h>

using namespace DirectX;

namespace Salt2D::Game::Director {

void StageCameraDirector::Initialize(
    StageWorld* world,
    const Story::StoryTables* tables,
    Render::Scene3D::Camera3D* camera
) {
    world_ = world;
    tables_ = tables;
    camera_ = camera;
}

void StageCameraDirector::Tick(const Story::StoryPlayer& player, const Core::FrameTime& /*ft*/) {
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

        XMFLOAT3 eye{
            anchor.head.x - anchor.outward.x * cfg_.dist,
            anchor.head.y + cfg_.liftY,
            anchor.head.z - anchor.outward.z * cfg_.dist
        };

        camera_->SetPosition(eye);
        camera_->LookAt(anchor.head);

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

        XMFLOAT3 eye{
            anchor.head.x - anchor.outward.x * cfg_.dist,
            anchor.head.y + cfg_.liftY,
            anchor.head.z - anchor.outward.z * cfg_.dist
        };

        camera_->SetPosition(eye);
        camera_->LookAt(anchor.head);

        break;
    }
    default:
        break;
    }
}

} // namespace Salt2D::Game::Director
