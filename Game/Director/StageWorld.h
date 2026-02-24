// Game/Director/StageWorld.h
#ifndef GAME_DIRECTOR_STAGEWORLD_H
#define GAME_DIRECTOR_STAGEWORLD_H

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <span>
#include <DirectXMath.h>

#include "Render/Draw/CardDrawItem.h"
#include "Render/Scene3D/Camera3D.h"
#include "Game/RenderBridge/TextureCatalog.h"
#include "Game/Story/StoryTables.h"
#include "Utils/Logger.h"
#include "Core/Time/FrameClock.h"

namespace Salt2D::Render {
    class DrawList;
} // namespace Salt2D::Render

namespace Salt2D::Game::Director {

struct Anchor {
    DirectX::XMFLOAT3 head{};
    DirectX::XMFLOAT3 face{};       // face toward center
    DirectX::XMFLOAT3 radialOut{};  // face outward
};

class StageWorld {
public:
    void Initialize(const Story::StoryTables* tables, RenderBridge::TextureCatalog* catalog);

    void LoadStage(const RHI::DX11::DX11Device& device, std::string_view stageId);

    void Update(const Core::FrameTime& /*ft*/) {}

    std::span<const Render::CardDrawItem> Cards() const { return cards_; }

    bool FindAnchor(const std::string_view& castId, Anchor& outHead) const;

    void EmitBackground(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) const;

private:
    const Story::StoryTables* tables_ = nullptr;
    RenderBridge::TextureCatalog* catalog_ = nullptr;

    std::vector<Render::CardDrawItem> cards_;
    std::unordered_map<std::string, Anchor> anchors_; // by cast ID

    RenderBridge::TextureRef bgTex_{};
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_STAGEWORLD_H
