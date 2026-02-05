// Game/UI/VnHud.cpp
#include "VnHud.h"
#include <algorithm>

namespace Salt2D::Game::UI {

const VnHudDrawData& VnHud::Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH) {
    drawData_ = {}; // Reset
    drawData_.visible = model.visible;
    if (!model.visible) return drawData_;

    const float margin = cfg_.margin;
    const float boxH = (std::min)(cfg_.maxBoxH, canvasH * cfg_.boxHRatio);

    drawData_.panel.x = margin;
    drawData_.panel.y = static_cast<float>(canvasH - margin - boxH);
    drawData_.panel.w = static_cast<float>(canvasW - margin * 2.0f);
    drawData_.panel.h = boxH;
    
    drawData_.panelTint = cfg_.panelTint;

    const float x = drawData_.panel.x + cfg_.pad;
    float y = drawData_.panel.y + cfg_.pad;

    const float innerW = (std::max)(1.0f, drawData_.panel.w - cfg_.pad * 2.0f);
    const float innerH = (std::max)(1.0f, drawData_.panel.h - cfg_.pad * 2.0f);

    // Speaker
    drawData_.speaker.style = TextStyleId::Speaker;
    drawData_.speaker.textUtf8 = model.speakerUtf8;
    drawData_.speaker.layoutW = innerW;
    drawData_.speaker.layoutH = cfg_.speakerH;
    drawData_.speaker.x = x;
    drawData_.speaker.y = y;

    y += cfg_.speakerH + cfg_.speakerGap;

    // Body
    drawData_.body.style = TextStyleId::Body;
    drawData_.body.textUtf8 = model.bodyUtf8;
    drawData_.body.layoutW = innerW;
    drawData_.body.layoutH = (std::max)(1.0f, innerH - cfg_.speakerH - cfg_.speakerGap);
    drawData_.body.x = x;
    drawData_.body.y = y;
    
    return drawData_;
}

} // namespace Salt2D::Game::UI
