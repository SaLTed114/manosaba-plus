// Game/UI/PresentHud.cpp
#include "PresentHud.h"
#include <algorithm>

namespace Salt2D::Game::UI {

static inline int ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

const PresentHudDrawData& PresentHud::Build(const PresentHudModel& model, uint32_t canvasW, uint32_t canvasH) {
    draw_ = {};
    draw_.visible = model.visible;
    if (!draw_.visible) return draw_;

    const float margin = cfg_.margin;
    const float pad    = cfg_.pad;

    const float panelW = std::min(cfg_.panelW, (std::max)(1.0f, (float)canvasW - margin * 2.0f));

    const float itemsH = model.items.empty() ? 0.0f
        : (model.items.size() * cfg_.rowH + (model.items.size() - 1) * cfg_.rowGap);

    const float panelH = pad * 2.0f + cfg_.promptH + (model.items.empty() ? 0.0f : cfg_.promptGap) + itemsH;

    draw_.panel.w = panelW;
    draw_.panel.h = std::min(panelH, (std::max)(1.0f, (float)canvasH - margin * 2.0f));

    draw_.panel.x = ((float)canvasW - draw_.panel.w) * 0.5f;
    draw_.panel.y = (std::max)(margin, (float)canvasH - margin - draw_.panel.h);

    draw_.panelTint = cfg_.panelTint;
    draw_.highlightTint = cfg_.highlightTint;

    const float innerX = draw_.panel.x + pad;
    const float innerY = draw_.panel.y + pad;
    const float innerW = (std::max)(1.0f, draw_.panel.w - pad * 2.0f);

    // prompt
    draw_.prompt.x = innerX;
    draw_.prompt.y = innerY;
    draw_.prompt.layoutW = innerW;
    draw_.prompt.layoutH = cfg_.promptH;
    draw_.prompt.textUtf8 = model.promptUtf8;
    draw_.prompt.style = TextStyleId::PresentPrompt;

    // items
    draw_.items.clear();
    draw_.highlightRects.clear();

    const float listY = innerY + cfg_.promptH + (model.items.empty() ? 0.0f : cfg_.promptGap);
    for (size_t i = 0; i < model.items.size(); ++i) {
        TextRequest tr{};
        tr.x = innerX;
        tr.y = listY + (float)i * (cfg_.rowH + cfg_.rowGap);
        tr.layoutW = innerW;
        tr.layoutH = cfg_.rowH;
        tr.textUtf8 = model.items[i].second;
        tr.style = TextStyleId::PresentSmall;

        draw_.items.push_back(std::move(tr));
    }

    if (!model.items.empty()) {
        const int sel = ClampWarp(model.selectedItem, (int)model.items.size());
        Render::RectF hl{};
        hl.x = innerX - 6.0f;
        hl.y = draw_.items[sel].y - 2.0f;
        hl.w = innerW + 12.0f;
        hl.h = cfg_.rowH + 4.0f;
        draw_.highlightRects.push_back(hl);
    }

    return draw_;
}

} // namespace Salt2D::Game::UI
