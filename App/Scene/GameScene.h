// App/Scene/GameScene.h
#ifndef APP_SCENE_GAMESCENE_H
#define APP_SCENE_GAMESCENE_H

#include <memory>
#include <string>
#include <DirectXMath.h>

#include "Render/DX11Renderer.h"
#include "Render/RenderPlan.h"
#include "Render/Draw/DrawList.h"
#include "Render/Scene3D/Camera3D.h"
#include "RHI/DX11/DX11Texture2D.h"

#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"

#include "Utils/DiskFileSystem.h"

#include "Game/Session/StorySession.h"
#include "Game/Session/StoryScreenManager.h"
#include "Game/RenderBridge/TextService.h"
#include "Game/RenderBridge/TextureService.h"
#include "Game/RenderBridge/TextureCatalog.h"
#include "Game/Director/StageWorld.h"
#include "Game/Director/StageCameraDirector.h"

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
    Utils::DiskFileSystem& fs_;

    Game::Session::StorySession session_;
    Game::Session::StoryScreenManager screens_;
    Game::RenderBridge::TextService text_;
    Game::RenderBridge::TextureService texService_; // for UI
    Game::RenderBridge::TextureCatalog texCatalog_; // for story screens

    RHI::DX11::DX11Texture2D white1x1_;
    RHI::DX11::DX11Texture2D checker_;

    Game::Director::StageWorld stage_;

    Game::Director::StageCameraDirector director_;
    Render::Scene3D::Camera3D camera_;
};

} // namespace Salt2D::App

#endif // APP_SCENE_GAMESCENE_H
