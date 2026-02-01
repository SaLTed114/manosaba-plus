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
#include "RHI/DX11/DX11Texture2D.h"
#include "Core/Time/FrameClock.h"

namespace Salt2D::App {

class DemoScene {
public:
    void Initialize(Render::DX11Renderer& renderer);
    void Update(const Core::FrameTime& ft, uint32_t canvasW, uint32_t canvasH);
    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH);

    void BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH);
    void BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList);

private:
    RHI::DX11::DX11Texture2D checker_;
    RHI::DX11::DX11Texture2D platform_;
    RHI::DX11::DX11Texture2D img1_;
    RHI::DX11::DX11Texture2D img2_;

    Render::Scene3D::Camera3D camera_;
    std::vector<Render::MeshDrawItem> meshItems_;
    std::vector<Render::CardDrawItem> cardItems_;
    float angle_ = 0.0f;
};

} // namespace Salt2D::App

#endif // APP_SCENE_DEMOSCENE_H
