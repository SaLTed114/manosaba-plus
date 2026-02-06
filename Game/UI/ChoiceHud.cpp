// Game/UI/ChoiceHud.cpp
#include "ChoiceHud.h"
#include <algorithm>

namespace Salt2D::Game::UI {

static inline int ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

const ChoiceHudDrawData& ChoiceHud::Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH) {
    draw_ = {};
    draw_.visible = model.visible;
    if (!draw_.visible) return draw_;

    const float margin = cfg_.margin;
    const float pad    = cfg_.pad;

    const float panelW = std::min(cfg_.panelW, (std::max)(1.0f, (float)canvasW - margin * 2.0f));

    const float optionsH = model.options.empty() ? 0.0f
        : (model.options.size() * cfg_.optionH + (model.options.size() - 1) * cfg_.optionGap);

    const float panelH = pad * 2.0f + optionsH;

    draw_.panel.w = panelW;
    draw_.panel.h = std::min(panelH, (std::max)(1.0f, (float)canvasH - margin * 2.0f));

    draw_.panel.x = ((float)canvasW - draw_.panel.w) * 0.5f;
    draw_.panel.y = (std::max)(margin, (float)canvasH - margin - draw_.panel.h);

    draw_.panelTint = cfg_.panelTint;
    draw_.highlightTint = cfg_.highlightTint;

    const float innerX = draw_.panel.x + pad;
    const float innerY = draw_.panel.y + pad;
    const float innerW = (std::max)(1.0f, draw_.panel.w - pad * 2.0f);

    // options
    draw_.options.clear();
    draw_.highlightRects.clear();

    for (size_t i = 0; i < model.options.size(); ++i) {
        TextRequest tr{};
        tr.x = innerX;
        tr.y = innerY + (float)i * (cfg_.optionH + cfg_.optionGap);
        tr.layoutW = innerW;
        tr.layoutH = cfg_.optionH;
        tr.textUtf8 = model.options[i].second;
        tr.style = TextStyleId::ChoiceSmall;

        draw_.options.push_back(tr);
    }

    if (!model.options.empty()) {
        const int sel = ClampWarp(model.selectedOption, (int)model.options.size());
        Render::RectF hr{};
        hr.x = innerX;
        hr.y = innerY + (float)sel * (cfg_.optionH + cfg_.optionGap);
        hr.w = innerW;
        hr.h = cfg_.optionH;
        draw_.highlightRects.push_back(hr);
    }

    return draw_;
}

} // namespace Salt2D::Game::UI
