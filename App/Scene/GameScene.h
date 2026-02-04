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
#include "RHI/DX11/DX11Texture2D.h"

#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"

#include "Utils/DiskFileSystem.h"
#include "Game/Story/StoryGraph.h"
#include "Game/Story/StoryPlayer.h"

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
    static std::wstring Utf8ToWString(const std::string& str);

    void EnsureTextSystem();
    void UpdateUILayout(uint32_t canvasW, uint32_t canvasH);
    void BakeVNIfDirty(const RHI::DX11::DX11Device& device);

private:
    Utils::DiskFileSystem& fs_;
    Game::Story::StoryGraph graph_;
    std::unique_ptr<Game::Story::StoryPlayer> player_;

    Game::Logger logger_;

    uint32_t lastCanvasW_ = 0;
    uint32_t lastCanvasH_ = 0;

    Render::RectF vnBox_{};
    float vnPad_ = 16.0f;

    float speakerLayoutW_ = 0.0f;
    float speakerLayoutH_ = 0.0f;
    float bodyLayoutW_ = 0.0f;
    float bodyLayoutH_ = 0.0f;

    RHI::DX11::DX11Texture2D white1x1_;

    Render::Text::TextBaker textBaker_;
    bool textInited_ = false;

    Render::Text::TextStyle speakerStyle_;
    Render::Text::TextStyle bodyStyle_;

    Render::Text::BakedText speakerTex_;
    Render::Text::BakedText bodyTex_;

    std::string lastSpeaker_;
    std::string lastBody_;
    bool vnTextDirty_ = false;
};

} // namespace Salt2D::App

#endif // APP_SCENE_GAMESCENE_H
