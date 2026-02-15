// Game/UI/Widgets/VnDialogWidget.cpp
#include "VnDialogWidget.h"
#include "Utils/StringUtils.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

namespace {

static inline bool HasAny(UIFrame& frame, const FirstGlyphTextRef& ref) {
    return GetText(frame, ref.first) || GetText(frame, ref.rest);
}

} // Anonymous namespace

void VnDialogWidget::Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;

    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);

    const float margin = cfg_.margin;
    const float boxH = (std::min)(cfg_.maxBoxH, h * cfg_.boxHRatio);

    // Panel
    Render::RectF panel = RectXYWH(margin, h - margin - boxH, w - margin * 2.0f, boxH);
    PushSprite(frame, TextureId::White, panel, cfg_.panelTint, 0.0f, Render::Layer::HUD);

    // Speaker
    nameBaseX_ = cfg_.speakerScale.x * w;
    nameBaseY_ = cfg_.speakerScale.y * h;

    std::string family, given;
    Utils::SplitNameFamilyGiven(model.speakerUtf8, family, given);

    fam_ = AddFirstGlyphText(frame,
        TextStyleId::VnNameFamilyBig, TextStyleId::VnNameRest,
        family, nameBaseX_, nameBaseY_, w, cfg_.speakerH,
        model.color, cfg_.nameRestTint, 0.25f, cfg_.nameSegGap);

    giv_ = AddFirstGlyphText(frame,
        TextStyleId::VnNameGivenBig, TextStyleId::VnNameRest,
        given, nameBaseX_, nameBaseY_, w, cfg_.speakerH,
        cfg_.nameRestTint, cfg_.nameRestTint, 0.25f, cfg_.nameSegGap);

    // Body
    bodyTextIdx_ = PushText(frame, TextStyleId::VnBody, model.bodyUtf8,
        cfg_.bodyScale.x * w, cfg_.bodyScale.y * h,
        (0.75f - cfg_.bodyScale.x) * w, (1.0f - cfg_.bodyScale.y) * h,
        Render::Color4F{1,1,1,1}, 0.2f, Render::Layer::HUD);

    if (TextOp* bodyOp = GetText(frame, bodyTextIdx_)) {
        bodyOp->revealEnabled = true;
        bodyOp->revealU01 = model.bodyRevealU01;
        bodyOp->revealSoftPx = 108.0f;
        bodyOp->revealSoftStep = 12;
    }
}

void VnDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    TextOp* f1 = GetText(frame, fam_.first);
    TextOp* fr = GetText(frame, fam_.rest);
    TextOp* g1 = GetText(frame, giv_.first);
    TextOp* gr = GetText(frame, giv_.rest);
    if (!f1 && !fr && !g1 && !gr) return; // no name text, skip

    float maxH = 0.0f;
    auto UpdateMaxH = [&](TextOp* op) {
        if (!op) return;
        maxH = (std::max)(maxH, static_cast<float>(op->baked.h));
    };
    UpdateMaxH(f1); UpdateMaxH(fr);
    UpdateMaxH(g1); UpdateMaxH(gr);

    float x = nameBaseX_;
    float y = nameBaseY_;

    auto PlaceGroup = [&](const FirstGlyphTextRef& ref) {
        TextOp* firstOp = GetText(frame, ref.first);
        TextOp* restOp  = GetText(frame, ref.rest);

        auto Place = [&](TextOp* op) {
            if (!op) return;
            op->x = x;
            op->y = y + maxH - static_cast<float>(op->baked.h);
            x += static_cast<float>(op->baked.w);
        };

        Place(firstOp);
        if (firstOp && restOp) x += ref.gapPx;
        Place(restOp);
    };

    const bool hasFamily = HasAny(frame, fam_);
    const bool hasGiven  = HasAny(frame, giv_);

    PlaceGroup(fam_);
    if (hasFamily && hasGiven) x += cfg_.namePartGap;
    PlaceGroup(giv_);
}

} // namespace Salt2D::Game::UI
