// Game/UI/Widgets/VnDialogWidget.cpp
#include "VnDialogWidget.h"
#include "Utils/StringUtils.h"
#include "Utils/MathUtils.h"

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
    panelOp.texId = TextureId::Checker;
    panelOp.srv = nullptr;
    panelOp.dst = panel;
    panelOp.tint = Render::Color4F{1,1,1,1};//cfg_.panelTint;
    panelOp.z = 0.0f;
    frame.sprites.push_back(std::move(panelOp));

    const float x = panel.x + cfg_.pad;
    float y = panel.y + cfg_.pad;

    const float innerW = Utils::EnsureFinite(panel.w - cfg_.pad * 2.0f);
    const float innerH = Utils::EnsureFinite(panel.h - cfg_.pad * 2.0f);

    // Speaker
    idxFam1_   = -1; idxFamRest_   = -1;
    idxGiven1_ = -1; idxGivenRest_ = -1;
    nameBaseX_ = x; nameBaseY_ = y;

    std::string family, given;
    Utils::SplitNameFamilyGiven(model.speakerUtf8, family, given);

    std::string fam1, famRest, giv1, givRest;
    Utils::SplitFirstCp(family, fam1, famRest);
    Utils::SplitFirstCp(given,  giv1, givRest);

    auto PushText = [&](TextStyleId id, std::string str, Render::Color4F tint) {
        if (str.empty()) return -1;
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = id;
        op.textUtf8 = std::move(str);

        op.layoutW = innerW; // will be updated in AfterBake
        op.layoutH = cfg_.speakerH; // will be updated in AfterBake

        op.x = nameBaseX_;
        op.y = nameBaseY_;
        op.tint = tint;
        op.z = 0.2f;

        int idx = static_cast<int>(frame.texts.size());
        frame.texts.push_back(std::move(op));
        return idx;
    };

    const auto accent = cfg_.nameAccentTint;
    idxFam1_      = PushText(TextStyleId::VnNameFamilyBig, std::move(fam1),    accent);
    idxFamRest_   = PushText(TextStyleId::VnNameRest,      std::move(famRest), Render::Color4F{1,1,1,1});
    idxGiven1_    = PushText(TextStyleId::VnNameGivenBig,  std::move(giv1),    Render::Color4F{1,1,1,1});
    idxGivenRest_ = PushText(TextStyleId::VnNameRest,      std::move(givRest), Render::Color4F{1,1,1,1});

    y += cfg_.speakerH + cfg_.speakerGap;

    // Body
    TextOp bodyOp;
    bodyOp.layer = Render::Layer::HUD;
    bodyOp.styleId = TextStyleId::VnBody;
    bodyOp.textUtf8 = model.bodyUtf8;
    bodyOp.layoutW = innerW;
    bodyOp.layoutH = Utils::EnsureFinite(innerH - cfg_.speakerH - cfg_.speakerGap);
    
    bodyOp.x = x; bodyOp.y = y;
    bodyOp.z = 0.2f;
    frame.texts.push_back(std::move(bodyOp));

    // Hit area
    // placeholder: whole panel is clickable
}

void VnDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    auto& texts = frame.texts;
    auto GetText = [&](int idx) -> TextOp* {
        if (idx < 0 || static_cast<size_t>(idx) >= texts.size()) return nullptr;
        return &texts[idx];
    };

    TextOp* opFam1 = GetText(idxFam1_);
    TextOp* opFamR = GetText(idxFamRest_);
    TextOp* opGiv1 = GetText(idxGiven1_);
    TextOp* opGivR = GetText(idxGivenRest_);

    if (!opFam1 && !opFamR && !opGiv1 && !opGivR) return; // no name text, skip

    float maxH = 0.0f;
    auto UpdateMaxH = [&](TextOp* op) {
        if (!op) return;
        maxH = (std::max)(maxH, static_cast<float>(op->baked.h));
    };
    UpdateMaxH(opFam1); UpdateMaxH(opFamR);
    UpdateMaxH(opGiv1); UpdateMaxH(opGivR);

    auto AlignBottom = [&](TextOp* op) {
        if (!op) return;
        op->y = nameBaseY_ + maxH - static_cast<float>(op->baked.h);
    };
    AlignBottom(opFam1); AlignBottom(opFamR);
    AlignBottom(opGiv1); AlignBottom(opGivR);

    float x = nameBaseX_;

    auto Place = [&](TextOp* op, float gap) {
        if (!op) return;
        op->x = x;
        x += static_cast<float>(op->baked.w) + gap;
    };

    // family
    Place(opFam1, opFamR ? cfg_.nameSegGap : 0.0f);
    Place(opFamR, 0.0f);

    const bool hasFamily = (opFam1 || opFamR);
    const bool hasGiven  = (opGiv1 || opGivR);
    if (hasFamily && hasGiven) x += cfg_.namePartGap; // extra gap between family and given

    // given
    Place(opGiv1, opGivR ? cfg_.nameSegGap : 0.0f);
    Place(opGivR, 0.0f);
}

} // namespace Salt2D::Game::UI
