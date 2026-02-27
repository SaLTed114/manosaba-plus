// App/Scene/PlayScene.h
#ifndef APP_SCENE_PLAYSCENE_H
#define APP_SCENE_PLAYSCENE_H

#include <memory>
#include <vector>
#include <filesystem>
#include <DirectXMath.h>

#include "Render/DX11Renderer.h"
#include "Render/RenderPlan.h"
#include "Render/Draw/DrawList.h"
#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"
#include "Utils/DiskFileSystem.h"

#include "Game/Flow/GameFlow.h"
#include "Game/Flow/GameFlowTypes.h"
#include "Game/Session/StoryScreenManager.h"
#include "Game/RenderBridge/TextService.h"
#include "Game/RenderBridge/TextureService.h"
#include "Game/RenderBridge/TextureCatalog.h"

#include "RHI/DX11/DX11Texture2D.h"

namespace Salt2D::App {

class PlayScene {
public:
    explicit PlayScene(Utils::DiskFileSystem& fs);

    void Initialize(Render::DX11Renderer& renderer);

    void Update(const RHI::DX11::DX11Device& device,
        const Core::FrameTime& ft, const Core::InputState& in,
        uint32_t canvasW, uint32_t canvasH);

    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH);

    void BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH);
    void BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList);

private:
    void BuildDefaultChapters();
    void ProcessFlowEvents(const RHI::DX11::DX11Device& device);

    void BindToFlowSession();
    void UnbindFromFlowSession();

private:
    Utils::DiskFileSystem& fs_;

    Game::Flow::GameFlow flow_;
    Game::Session::StoryScreenManager screens_;
    bool screensBound_ = false;

    Game::RenderBridge::TextService text_;
    Game::RenderBridge::TextureService texService_;
    Game::RenderBridge::TextureCatalog texCatalog_;

    RHI::DX11::DX11Texture2D white1x1_;
    RHI::DX11::DX11Texture2D checker_;
};

} // namespace Salt2D::App

#endif // APP_SCENE_PLAYSCENE_H
