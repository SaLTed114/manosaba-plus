// Game/UI/DebateHud.cpp
#include "DebateHud.h"
#include <algorithm>

namespace Salt2D::Game::UI {

static inline std::string SpanLine(int i, const std::string& spanId, bool opened) {
    // [1] Q1  (OPEN)
    std::string line = "[" + std::to_string(i + 1) + "] " + spanId;
    if (opened) line += "  (OPEN)";
    return line;
}

static inline std::string OptionLine(int i, const std::string& label) {
    //     [1] Agree
    return "    [" + std::to_string(i) + "] " + label;
}

const DebateHudDrawData& DebateHud::Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH) {
    drawData_ = {}; // Reset
    drawData_.visible = model.visible;
    if (!model.visible) return drawData_;

    const float margin = cfg_.margin;
    const float panelW = (std::min)(cfg_.panelW, static_cast<float>(canvasW - margin * 2.0f));

    drawData_.panel.x = static_cast<float>(canvasW) - margin - panelW;
    drawData_.panel.y = margin;
    drawData_.panel.w = panelW;
    drawData_.panel.h = static_cast<float>(canvasH) - margin * 2.0f;
    drawData_.panelTint = cfg_.panelTint;
    
    const float x0 = drawData_.panel.x + cfg_.pad;
    float y = drawData_.panel.y + cfg_.pad;

    const float innerW = (std::max)(1.0f, drawData_.panel.w - cfg_.pad * 2.0f);
    const float innerH = (std::max)(1.0f, drawData_.panel.h - cfg_.pad * 2.0f);

    // Speaker
    drawData_.speaker.style = TextStyleId::DebateSpeaker;
    drawData_.speaker.textUtf8 = model.speakerUtf8;
    drawData_.speaker.layoutW = innerW;
    drawData_.speaker.layoutH = cfg_.speakerH;
    drawData_.speaker.x = x0;
    drawData_.speaker.y = y;

    y += cfg_.speakerH + cfg_.speakerGap;

    // Body
    const float bodyH = (std::min)(220.0f, innerH * 0.4f);
    drawData_.body.style = TextStyleId::DebateBody;
    drawData_.body.textUtf8 = model.bodyUtf8;
    drawData_.body.layoutW = innerW;
    drawData_.body.layoutH = bodyH;
    drawData_.body.x = x0;
    drawData_.body.y = y;

    y += bodyH + cfg_.selectionGap;

    // Spans
    drawData_.spans.clear();
    drawData_.highlightRects.clear();

    const int spanCount = static_cast<int>(model.spanIds.size());
    for (int i = 0; i < spanCount; ++i) {
        bool opened = (model.menuOpen && model.openedSpanId == model.spanIds[i]);
        TextRequest spanReq;
        spanReq.style = TextStyleId::DebateSmall;
        spanReq.textUtf8 = SpanLine(i, model.spanIds[i], opened);
        spanReq.layoutW = innerW;
        spanReq.layoutH = cfg_.rowH;
        spanReq.x = x0;
        spanReq.y = y + i * cfg_.rowH;
        drawData_.spans.push_back(std::move(spanReq));

        if (i == model.selectedSpan) {
            Render::RectF hlRect;
            hlRect.x = drawData_.panel.x + 6.0f;
            hlRect.y = spanReq.y;
            hlRect.w = drawData_.panel.w - 12.0f;
            hlRect.h = cfg_.rowH;
            drawData_.highlightRects.push_back(std::move(hlRect));
        }
    }
    y += spanCount * cfg_.rowH + cfg_.selectionGap;

    if (model.menuOpen) {
        // Options
        drawData_.options.clear();

        const int optCount = static_cast<int>(model.menuOptions.size());
        for (int i = 0; i < optCount; ++i) {
            const auto& [optionId, label] = model.menuOptions[i];

            TextRequest optReq;
            optReq.style = TextStyleId::DebateSmall;
            optReq.textUtf8 = OptionLine(i, label);
            optReq.layoutW = innerW;
            optReq.layoutH = cfg_.rowH;
            optReq.x = x0;
            optReq.y = y + i * cfg_.rowH;
            drawData_.options.push_back(std::move(optReq));

            if (i == model.selectedOpt) {
                Render::RectF hlRect;
                hlRect.x = drawData_.panel.x + 6.0f;
                hlRect.y = optReq.y;
                hlRect.w = drawData_.panel.w - 12.0f;
                hlRect.h = cfg_.rowH;
                drawData_.highlightRects.push_back(std::move(hlRect));
            }
        }
    }

    return drawData_;
}
    
} // namespace Salt2D::Game::UI
