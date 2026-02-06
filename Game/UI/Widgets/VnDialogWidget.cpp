// Game/UI/Widgets/VnDialogWidget.cpp
#include "VnDialogWidget.h"

namespace Salt2D::Game::UI {

void VnDialogWidget::Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;

    const float margin = cfg_.margin;
    const float boxH = (std::min)(cfg_.maxBoxH, canvasH * cfg_.boxHRatio);

    // Panel
    Render::RectF panel;
    panel.x = margin;
    panel.y = static_cast<float>(canvasH - margin - boxH);
    panel.w = static_cast<float>(canvasW - margin * 2.0f);
    panel.h = boxH;

    SpriteOp panelOp;
    panelOp.layer = Render::Layer::HUD;
    panelOp.srv = nullptr;
    panelOp.dst = panel;
    panelOp.tint = cfg_.panelTint;
    panelOp.z = 0.0f;
    frame.sprites.push_back(std::move(panelOp));

    const float x = panel.x + cfg_.pad;
    float y = panel.y + cfg_.pad;

    const float innerW = (std::max)(1.0f, panel.w - cfg_.pad * 2.0f);
    const float innerH = (std::max)(1.0f, panel.h - cfg_.pad * 2.0f);

    // Speaker
    TextOp speakerOp;
    speakerOp.layer = Render::Layer::HUD;
    speakerOp.styleId = TextStyleId::VnSpeaker;
    speakerOp.textUtf8 = model.speakerUtf8;
    speakerOp.layoutW = innerW;
    speakerOp.layoutH = cfg_.speakerH;
    speakerOp.x = x; speakerOp.y = y;
    speakerOp.z = 0.2f;
    frame.texts.push_back(std::move(speakerOp));

    y += cfg_.speakerH + cfg_.speakerGap;

    // Body
    TextOp bodyOp;
    bodyOp.layer = Render::Layer::HUD;
    bodyOp.styleId = TextStyleId::VnBody;
    bodyOp.textUtf8 = model.bodyUtf8;
    bodyOp.layoutW = innerW;
    bodyOp.layoutH = (std::max)(1.0f, innerH - cfg_.speakerH - cfg_.speakerGap);
    bodyOp.x = x; bodyOp.y = y;
    bodyOp.z = 0.2f;
    frame.texts.push_back(std::move(bodyOp));

    // Hit area
    // placeholder: whole panel is clickable
}

} // namespace Salt2D::Game::UI
