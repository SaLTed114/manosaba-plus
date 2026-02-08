// Game/UI/Widgets/DebateDialogWidget.cpp
#include "DebateDialogWidget.h"
#include "Utils/MathUtils.h"
#include "Utils/StringUtils.h"
#include "Game/Story/TextMarkup/SusMarkup.h"

#include <unordered_map>
#include <iostream>

namespace Salt2D::Game::UI {

void DebateDialogWidget::Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!visible_) return;

    pieces_.clear();
    lineBegin_.clear();
    lineCount_.clear();

    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);

    // TODO: 文字布局应当由 model 生成（几种固定的文字轨道，由screen决定当前文字用哪个轨道并交给 model ）

    baseX_ = w * 0.2f;
    baseY_ = h * 0.3f;
    const float maxW = w * 0.6f;
    const float maxH = h * 0.5f;

    const size_t spanCount = model.spanIds.size();
    std::unordered_map<std::string, int> spanMap;
    spanMap.reserve(spanCount);
    for (int i = 0; i < static_cast<int>(spanCount); ++i) {
        spanMap[model.spanIds[i]] = i;
    }

    auto parsed = Story::ParseSusMarkup(model.bodyUtf8);
    if (!parsed.ok) { PushText(frame, TextStyleId::DebateBody, parsed.plainTextUtf8,
        baseX_, baseY_, maxW, maxH, cfg_.bodyTint); return; }


    struct Ctx {
        DebateDialogWidget& self;
        UIFrame& frame;
        float x0, y0, layoutW, layoutH;
        const DebateHudModel& model;
        const std::unordered_map<std::string, int>& spanMap;
        int line = 0;

        void EnsureLine(int lineIndex) {
            while (lineIndex >= static_cast<int>(self.lineBegin_.size())) {
                self.lineBegin_.push_back(static_cast<int>(self.pieces_.size()));
                self.lineCount_.push_back(0);
            }
        }

        void AddPiece(const Piece& piece) {
            EnsureLine(line);
            self.pieces_.push_back(piece);
            self.lineCount_[line]++;
        }

        void PushSegment(std::string seg, bool isSus, std::string_view spanId) {
            if (seg.empty()) return;
            const TextStyleId styleId = isSus ? TextStyleId::DebateSus : TextStyleId::DebateBody;
            const Render::Color4F tint = isSus ? self.cfg_.susTint : self.cfg_.bodyTint;
            int textIdx = PushText(frame, styleId, std::move(seg), x0, y0, layoutW, layoutH, tint);

            Piece piece{ .textIdx = textIdx, .isSus = isSus };
            if (isSus) {
                auto it = spanMap.find(std::string(spanId));
                if (it != spanMap.end()) {
                    piece.spanIdx = it->second;
                    piece.hitIdx = PushHit(frame,
                        MakeHitKey(HitKind::DebateSpan, piece.spanIdx),
                        Render::RectF{0,0,0,0}); // will update after bake
                } else {
                    // TODO: warn
                }
            }

            AddPiece(piece);
        }

        void PushRun(const Story::SusRun& run) {
            const bool isSus = run.kind == Story::SusRun::Kind::Sus;
            std::string_view seg(run.textUtf8);

            size_t pos = 0;
            EnsureLine(line);
            while (pos < seg.size()) {
                size_t next = seg.find('\n', pos);
                if (next == std::string_view::npos) {
                    PushSegment(std::string(seg.substr(pos)), isSus, run.spanId);
                    break;
                } else {
                    PushSegment(std::string(seg.substr(pos, next - pos)), isSus, run.spanId);
                    pos = next + 1;
                    line++;
                    EnsureLine(line);
                }
            }
        }
    };

    Ctx ctx{ *this, frame, baseX_, baseY_, maxW, maxH, model, spanMap };
    for (const auto& run : parsed.runs) ctx.PushRun(run);
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

            if (piece.isSus && piece.hitIdx >= 0) {
                SetHitRectFromTextAABB(frame, piece.hitIdx, piece.textIdx);
            }
        }

        y += maxLineH + lineGap_;
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
            op->tint = cfg_.susTint;
        }
    }
}

} // namespace Salt2D::Game::UI
