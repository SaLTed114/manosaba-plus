// Game/Director/HudEvaluator.cpp
#include "HudEvaluator.h"

#include "Game/Story/StoryTables.h"
#include "Game/Story/StoryView.h"
#include "Game/UI/UITypes.h"

#include <cmath>
#include <algorithm>
#include <iostream>

constexpr float pi = 3.14159265358979323846f;
constexpr float pi_div2 = 0.5f * pi;

namespace Salt2D::Game::Director {

static inline float Clamp01(float v) { return (std::max)(0.0f, (std::min)(1.0f, v)); }

static inline float KeepUpright(float rad) {
    if (rad >  pi_div2) rad -= pi;
    if (rad < -pi_div2) rad += pi;
    return rad;
}

static inline void ResolvePointPx(
    const Story::DebateTextTrack2D& track,
    uint32_t canvasW, uint32_t canvasH,
    float inX, float inY, float& outX, float& outY
) {
    if (track.space == Story::TrackSpace::Normalized) {
        outX = inX * static_cast<float>(canvasW);
        outY = inY * static_cast<float>(canvasH);
    } else {
        outX = inX;
        outY = inY;
    }
}

static float EvalTrapezoidAlpha(float totalSec, float remainSec, float fadeInSec, float fadeOutSec) {
    if (totalSec <= 1e-4f) return 1.0f;

    float t = totalSec - remainSec;
    t = std::clamp(t, 0.0f, totalSec);

    fadeInSec  = (std::max)(fadeInSec,  0.0f);
    fadeOutSec = (std::max)(fadeOutSec, 0.0f);

    const float sum = fadeInSec + fadeOutSec;
    if (sum > 1e-4 && sum > totalSec) {
        // scale down fade times if they exceed total duration
        const float scale = totalSec / sum;
        fadeInSec  *= scale;
        fadeOutSec *= scale;
    }

    if (fadeInSec > 1e-4f && t < fadeInSec) {
        return Clamp01(t / fadeInSec);
    }
    if (fadeOutSec > 1e-4f && t > totalSec - fadeOutSec) {
        return Clamp01((totalSec - t) / fadeOutSec);
    }

    return 1.0f;
}

UI::HudPose2D DefaultDebateDialogPose(uint32_t canvasW, uint32_t canvasH) {
    UI::HudPose2D pose;
    pose.baseX = static_cast<float>(canvasW) * 0.2f;
    pose.baseY = static_cast<float>(canvasH) * 0.3f;
    pose.rotRad = 0.0f;
    pose.alpha = 1.0f;
    return pose;
}

UI::HudPose2D EvalDebateDialogPose(
    const Story::StoryTables& tables,
    uint32_t canvasW, uint32_t canvasH,
    float totalSec, float remainSec,
    std::string_view prefId
) {
    auto pose = DefaultDebateDialogPose(canvasW, canvasH);

    const auto* pref = tables.perf.Find(prefId);
    if (!pref || !pref->hud.debateTextTrack.has_value()) return pose;

    const auto& track = pref->hud.debateTextTrack.value();

    float p = 0.0f;
    if (totalSec > 1e-4f) {
        const float ratio = Clamp01(remainSec / totalSec);
        p = 1.0f - ratio; // so that it starts at 0 and goes to 1
    }

    float startX, startY, endX, endY;
    ResolvePointPx(track, canvasW, canvasH, track.start.x, track.start.y, startX, startY);
    ResolvePointPx(track, canvasW, canvasH, track.end.x, track.end.y, endX, endY);

    pose.baseX = startX + (endX - startX) * p;
    pose.baseY = startY + (endY - startY) * p;

    float rotRad = 0.0f;

    switch (track.rotation.mode) {
    case Story::RotateMode::Slope: {
        const float dx = endX - startX;
        const float dy = endY - startY;

        rotRad = std::atan2(dy, dx);
        if (track.rotation.keepUpright) rotRad = KeepUpright(rotRad);
        break;
    }
    case Story::RotateMode::Fixed:
        rotRad = track.rotation.fixedRad;
        break;
    default:
        rotRad = 0.0f;
        break;
    }

    pose.rotRad = rotRad;

    const float fadeinSec  = (std::min)(0.12f, totalSec * 0.15f);
    const float fadeoutSec = (std::min)(0.12f, totalSec * 0.15f);
    pose.alpha = EvalTrapezoidAlpha(totalSec, remainSec, fadeinSec, fadeoutSec);

    return pose;
}

} // namespace Salt2D::Game::Director
