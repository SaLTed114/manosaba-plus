// Game/Screens/StoryOverlayLayer.cpp
#include "StoryOverlayLayer.h"

namespace Salt2D::Game::Screens {

void StoryOverlayLayer::Tick(const Core::FrameTime& /*ft*/, const Core::InputState& /*in*/, uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    if (!player_) return;

    const auto& view = player_->View().timer;

    timer_.Build(view, canvasW, canvasH, frame_);
}

void StoryOverlayLayer::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    timer_.AfterBake(frame_);
}

void StoryOverlayLayer::PostBake(const Core::InputState& /*in*/, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    if (!player_) return;

    // placeholder for future interaction handling
}

void StoryOverlayLayer::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;

    emitter_.Emit(drawList, service, frame_);
}

} // namespace Salt2D::Game::Screens
