// App/Scene/DemoScene.h
#ifndef APP_SCENE_DEMOSCENE_H
#define APP_SCENE_DEMOSCENE_H

#include <DirectXMath.h>

#include "Render/DX11Renderer.h"
#include "Render/Draw/DrawList.h"
#include "Render/Draw/MeshDrawItem.h"
#include "Render/Draw/CardDrawItem.h"
#include "Render/RenderPlan.h"
#include "Render/Scene3D/Camera3D.h"
#include "Render/Scene3D/Mesh.h"
#include "Render/Text/TextBaker.h"
#include "RHI/DX11/DX11Texture2D.h"
#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"

namespace Salt2D::App {

class DemoScene {
public:
    void Initialize(Render::DX11Renderer& renderer);
    void Update(const RHI::DX11::DX11Device& device, const Core::FrameTime& ft, const Core::InputState& in, uint32_t canvasW, uint32_t canvasH);
    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH);

    void BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH);
    void BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList);

private:
    RHI::DX11::DX11Texture2D checker_;
    RHI::DX11::DX11Texture2D platform_;
    RHI::DX11::DX11Texture2D img1_;
    RHI::DX11::DX11Texture2D img2_;
    Render::Text::BakedText bakedText_;

    Render::Scene3D::Camera3D camera_;
    std::vector<Render::MeshDrawItem> meshItems_;
    std::vector<Render::CardDrawItem> cardItems_;
    float angle_ = 0.0f;

    Render::Text::TextBaker textBaker_;
    bool textInited_ = false;

    std::wstring text_ = L"Hello, Salt2D!\n欢迎使用 Salt2D 引擎！";
    Render::Text::TextStyle style_;
    float layoutW_ = 512.0f;
    float layoutH_ = 256.0f;

    bool textDirty_ = true;
};

} // namespace Salt2D::App

#endif // APP_SCENE_DEMOSCENE_H
