// App/Scene/GameScene.h
#ifndef APP_SCENE_GAMESCENE_H
#define APP_SCENE_GAMESCENE_H

#include <memory>
#include <string>
#include <DirectXMath.h>

#include "Render/DX11Renderer.h"
#include "Render/RenderPlan.h"
#include "Render/Draw/DrawList.h"
#include "Render/Text/TextBaker.h"
#include "Render/Text/TextCache.h"
#include "RHI/DX11/DX11Texture2D.h"

#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"

#include "Utils/DiskFileSystem.h"
#include "Game/Story/StoryGraph.h"
#include "Game/Story/StoryPlayer.h"

#include "Game/UI/VnHud.h"
#include "Game/UI/DebateHud.h"

namespace Salt2D::App {

class GameScene {
public:
    explicit GameScene(Utils::DiskFileSystem& fs);

    void Initialize(Render::DX11Renderer& renderer);
    void Update(const RHI::DX11::DX11Device& device,
        const Core::FrameTime& ft, const Core::InputState& in,
        uint32_t canvasW, uint32_t canvasH);

    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH);

    void BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH);
    void BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList);

private:
    void EnsureTextSystem();

    void UpdateVN(const RHI::DX11::DX11Device& device,
        const Core::InputState& in,
        uint32_t canvasW, uint32_t canvasH);
    void UpdateDebate(const RHI::DX11::DX11Device& device,
        const Core::InputState& in,
        uint32_t canvasW, uint32_t canvasH);

    void HandleDebateInput(const Core::InputState& in, const Game::Story::StoryView::DebateView& view);
    void BuildDebateUI(uint32_t canvasW, uint32_t canvasH, const Game::Story::StoryView::DebateView& view);
    void BakeDebate(const RHI::DX11::DX11Device& device);

private:
    Render::Text::BakedText BakeRequiredText(
        const RHI::DX11::DX11Device& device,
        uint8_t styleId,
        const Render::Text::TextStyle& style,
        const Game::UI::TextRequest& req);

private:
    void DrawVN(Render::DrawList& drawList);
    void DrawDebate(Render::DrawList& drawList);

private:
    Utils::DiskFileSystem& fs_;
    Game::Story::StoryGraph graph_;
    std::unique_ptr<Game::Story::StoryPlayer> player_;

    Game::Logger logger_;

    Game::UI::VnHud vnHud_;
    Game::UI::VnHudDrawData vnDraw_;

    Game::UI::DebateHud debateHud_;
    Game::UI::DebateHudDrawData debateDraw_;

    int debateSelSpan_ = 0;
    int debateSelOption_ = 0;

private:
    Render::Text::TextCache textCache_;

    RHI::DX11::DX11Texture2D white1x1_;

    Render::Text::TextBaker textBaker_;
    bool textInited_ = false;

    Render::Text::TextStyle vnSpeakerStyle_;
    Render::Text::TextStyle vnBodyStyle_;

    Render::Text::BakedText vnSpeakerText_;
    Render::Text::BakedText vnBodyText_;

    Render::Text::TextStyle debateSpeakerStyle_;
    Render::Text::TextStyle debateBodyStyle_;
    Render::Text::TextStyle debateSmallStyle_;

    Render::Text::BakedText debateSpeakerText_;
    Render::Text::BakedText debateBodyText_;
    std::vector<Render::Text::BakedText> debateSpanTexts_;
    std::vector<Render::Text::BakedText> debateOptionTexts_;
};

} // namespace Salt2D::App

#endif // APP_SCENE_GAMESCENE_H
