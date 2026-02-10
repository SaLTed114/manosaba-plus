// Game/UI/Widgets/DebateDialogWidget.cpp
#include "DebateDialogWidget.h"
#include "Utils/MathUtils.h"
#include "Utils/StringUtils.h"

#include <unordered_map>
#include <iostream>

namespace Salt2D::Game::UI {

static inline Render::Color4F ApplyAlpha(const Render::Color4F& c, float alpha) {
    return Render::Color4F{c.r, c.g, c.b, c.a * alpha};
}

// ======================= Begin of text typeset helper =========================

static inline void ApplyGlobalPivotRotate(TextOp& op, float pivotPx, float pivotPy, float rotRad) {
    const float bw = static_cast<float>(op.baked.w);
    const float bh = static_cast<float>(op.baked.h);
    if (bw <= 0.0f || bh <= 0.0f) { op.transform.hasTransform = false; return; }

    op.transform.hasTransform = true;
    op.transform.rotRad = rotRad;
    op.transform.scaleX = 1.0f;
    op.transform.scaleY = 1.0f;

    op.transform.pivotX = (pivotPx - op.x) / bw;
    op.transform.pivotY = (pivotPy - op.y) / bh;
}

DebateDialogWidget::LayoutRegion DebateDialogWidget::ComputeLayout(uint32_t canvasW, uint32_t canvasH) {
    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);

    LayoutRegion region;
    region.x0 = baseX_;
    region.y0 = baseY_;
    region.w = w * 0.6f;
    region.h = h * 0.5f;
    return region;
}

void DebateDialogWidget::EnsureLine(int lineIdx) {
    while (lineIdx >= static_cast<int>(lineBegin_.size())) {
        lineBegin_.push_back(static_cast<int>(pieces_.size()));
        lineCount_.push_back(0);
    }
}

void DebateDialogWidget::AddPieceToLine(int lineIdx, const Piece& piece) {
    EnsureLine(lineIdx);
    pieces_.push_back(piece);
    lineCount_[lineIdx]++;
}

void DebateDialogWidget::PushSegment(
    UIFrame& frame, const DebateHudModel& model,
    const LayoutRegion& region, const std::string& segUtf8,
    bool isSus, std::string_view spanId, int lineIdx
) {
    if (segUtf8.empty()) return;

    const TextStyleId styleId = isSus ? TextStyleId::DebateSus : TextStyleId::DebateBody;
    const Render::Color4F tint = ApplyAlpha(isSus ? cfg_.susTint : cfg_.bodyTint, alpha_);
    int textIdx = PushText(frame, styleId, segUtf8, region.x0, region.y0, region.w, region.h, tint);

    Piece piece{ .textIdx = textIdx, .isSus = isSus };
    if (isSus) {
        auto it = spanMap_.find(spanId);
        if (it != spanMap_.end()) {
            piece.spanIdx = it->second;
            piece.hitIdx = PushHit(frame,
                MakeHitKey(HitKind::DebateSpan, piece.spanIdx),
                Render::RectF{0,0,0,0}, // will update after bake
                /*enabled=*/ !model.menuOpen,
                /*visible=*/true);
        } else {
            // TODO: warn
        }
    }

    AddPieceToLine(lineIdx, piece);
}

void DebateDialogWidget::PushRun(
    UIFrame& frame, const DebateHudModel& model,
    const LayoutRegion& region, const Story::SusRun& run, int& ioLine
) {
    const bool isSus = run.kind == Story::SusRun::Kind::Sus;
    std::string_view seg(run.textUtf8);

    EnsureLine(ioLine);
    size_t pos = 0;
    while (pos < seg.size()) {
        size_t next = seg.find('\n', pos);
        if (next == std::string_view::npos) {
            PushSegment(frame, model, region, std::string(seg.substr(pos)), isSus, run.spanId, ioLine);
            break;
        } else {
            PushSegment(frame, model, region, std::string(seg.substr(pos, next - pos)), isSus, run.spanId, ioLine);
            pos = next + 1;
            ioLine++;
            EnsureLine(ioLine);
        }
    }
}

void DebateDialogWidget::PushParsedRuns(
    UIFrame& frame, const DebateHudModel& model,
    const LayoutRegion& region, const Story::SusParseResult& parsed
) {
    int line = 0;
    EnsureLine(line);
    for (const auto& run : parsed.runs) {
        PushRun(frame, model, region, run, line);
    }
}

// ======================== End of text typeset helper =========================

void DebateDialogWidget::Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!visible_) return;

    spanCount_ = static_cast<int>(model.spanIds.size());

    pieces_.clear();
    lineBegin_.clear();
    lineCount_.clear();

    alpha_  = model.dialogPose.alpha;
    baseX_  = model.dialogPose.baseX;
    baseY_  = model.dialogPose.baseY;
    rotRad_ = model.dialogPose.rotRad;

    const LayoutRegion region = ComputeLayout(canvasW, canvasH);

    const size_t spanCount = static_cast<size_t>(spanCount_);
    spanMap_.clear();
    spanMap_.reserve(spanCount);
    for (int i = 0; i < static_cast<int>(spanCount); ++i) {
        spanMap_[model.spanIds[i]] = i;
    }

    auto parsed = Story::ParseSusMarkup(model.bodyUtf8);
    if (!parsed.ok) {
        auto tint = ApplyAlpha(cfg_.bodyTint, alpha_);
        PushText(frame, TextStyleId::DebateBody, parsed.plainTextUtf8,
            region.x0, region.y0, region.w, region.h, tint);
        return; 
    }

    PushParsedRuns(frame, model, region, parsed);
}

void DebateDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    float y = baseY_;
    for (size_t i = 0; i < lineBegin_.size(); ++i) {
        const int begin = lineBegin_[i];
        const int count = lineCount_[i];

        float maxLineH = 0.0f;
        for (int j = 0; j < count; ++j) {
            TextOp* op = GetText(frame, pieces_[begin + j].textIdx);
            if (!op) continue;
            maxLineH = (std::max)(maxLineH, static_cast<float>(op->baked.h));
        }

        float x = baseX_;
        for (int j = 0; j < count; ++j) {
            Piece& piece = pieces_[begin + j];
            TextOp* op = GetText(frame, piece.textIdx);
            if (!op) continue;
            
            op->x = x;
            op->y = y + (maxLineH - static_cast<float>(op->baked.h)) * 0.5f;
            x += static_cast<float>(op->baked.w);
            ApplyGlobalPivotRotate(*op, baseX_, baseY_, rotRad_);

            UpdateTextAABB(*op);
            if (piece.isSus && piece.hitIdx >= 0) {
                SetHitRectFromTextAABB(frame, piece.hitIdx, piece.textIdx);
            }
        }

        y += maxLineH + cfg_.lineGap;
    }
}

void DebateDialogWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;

    int hoveredSpan = -1;
    if (HitKeyKind(hoveredKey) == HitKind::DebateSpan) {
        hoveredSpan = static_cast<int>(HitKeyIndex(hoveredKey));
    }

    for (const auto& piece : pieces_) {
        if (!piece.isSus) continue;
        TextOp* op = GetText(frame, piece.textIdx);
        if (!op) continue;

        if (piece.spanIdx == hoveredSpan) {
            op->tint = cfg_.susHoverTint;
        } else {
            op->tint = ApplyAlpha(cfg_.susTint, alpha_);
        }
    }
}

bool DebateDialogWidget::TryPickSpan(HitKey clickedKey, int& outSpanIndex) const {
    if (HitKeyKind(clickedKey) != HitKind::DebateSpan) return false;
    const int spanIdx = static_cast<int>(HitKeyIndex(clickedKey));
    if (spanIdx < 0 || spanIdx >= spanCount_) return false;

    outSpanIndex = spanIdx;
    return true;
}

} // namespace Salt2D::Game::UI
