// Game/Director/StageWorld.cpp
#include "StageWorld.h"

#include "Render/Draw/DrawList.h"
#include "Render/Draw/SpriteDrawItem.h"

#include <cmath>
#include <algorithm>
#include <iostream>

using namespace DirectX;

namespace Salt2D::Game::Director {

static inline float WarpAngle(float angle) {
    // Warp angle to [-pi, pi]
    while (angle >  XM_PI) angle -= XM_2PI;
    while (angle < -XM_PI) angle += XM_2PI;
    return angle;
}

static inline void FitToHeight(
    uint32_t canvasW, uint32_t canvasH, uint32_t texW, uint32_t texH,
    float& outX, float& outY, float& outW, float& outH
) {
    if (texW == 0 || texW == 0 || canvasW == 0 || canvasH == 0) {
        outX = 0.0f; outY = 0.0f;
        outW = static_cast<float>(canvasW);
        outH = static_cast<float>(canvasH);
        return;
    }
    const float scale = static_cast<float>(canvasH) / static_cast<float>(texH);
    outW = static_cast<float>(texW) * scale;
    outH = static_cast<float>(texH) * scale;
    outX = (static_cast<float>(canvasW) - outW) * 0.5f;
    outY = 0.0f;
}

void StageWorld::Initialize(const Story::StoryTables* tables, RenderBridge::TextureCatalog* catalog) {
    tables_  = tables;
    catalog_ = catalog;

    cards_.clear();
    anchors_.clear();

    camera_.SetPosition({0.0f, 1.4f, 0.0f});
    camera_.SetFovY(60.0f * XM_PI / 180.0f);
    camera_.SetClip(0.1f, 100.0f);
    // camera_.LookAt({0.0f, 1.2f, 0.0f});
}

bool StageWorld::FindAnchor(const std::string_view& castId, DirectX::XMFLOAT3& outHead) const {
    const auto it = anchors_.find(std::string(castId));
    if (it == anchors_.end()) return false;
    outHead = it->second.head;
    return true;
}

void StageWorld::LoadStage(const RHI::DX11::DX11Device& device, std::string_view stageId) {
    cards_.clear();
    anchors_.clear();
    bgTex_ = {};

    if (!tables_ || !catalog_) return;

    const auto* stage = tables_->stage.Find(stageId);
    if (!stage) {
        // TODO: log warning about missing stage
        std::cout << "StageWorld::LoadStage: stage not found: " << stageId << std::endl;
        return;
    }

    // ==== background placeholder ====
    if (!stage->bgImage.empty()) {
        bgTex_ = catalog_->GetOrLoad(device, stage->bgImage);
    }

    // ==== ring layout ====
    const auto& ring = stage->ringLayout;
    const int N = (std::max)(1, ring.podiumCount);
    const float step = XM_2PI / static_cast<float>(N);

    const float cx = ring.centerX;
    const float cy = ring.centerY;
    const float cz = ring.centerZ;

    const float charRadius = ring.radius;
    const float podiumRadius = (std::max)(0.01f, charRadius - 0.5f);

    const float podiumWorldH = 1.0f;
    const float charWorldH = 1.8f;

    // ==== create podiums ====
    RenderBridge::TextureRef podiumTex{};
    podiumTex = catalog_->GetOrLoad(device, stage->podiumImage);
    if (!podiumTex.valid) {
        // TODO: log warning about missing podium texture
        // missing texture is set in catalog automatically
        std::cout << "StageWorld::LoadStage: podium texture not found: " << stage->podiumImage << std::endl;
    }

    cards_.reserve(static_cast<size_t>(N) + stage->slots.size());

    for (int i = 0; i < N; i++) {
        const float angle = step * static_cast<float>(i) + ring.yawOffsetRad;
        const float x = cx + podiumRadius * std::sinf(angle);
        const float z = cz + podiumRadius * std::cosf(angle);
        
        Render::CardDrawItem podium{};
        podium.srv = podiumTex.srv;
        podium.pos = {x, cy, z};
        podium.yaw = angle; // face outward
        
        float aspect = podiumTex.w > 0 ?
            static_cast<float>(podiumTex.w) /
            static_cast<float>(podiumTex.h) : 1.0f;
        podium.size = {podiumWorldH * aspect, podiumWorldH};

        podium.alphaCut = 0.5f;

        cards_.push_back(podium);
    }

    // ==== create character by slots ====
    for (const auto& slot : stage->slots) {
        const auto* cast = tables_->cast.FindById(slot.castId);
        if (!cast) {
            // TODO: log warning about missing cast for this slot
            std::cout << "StageWorld::LoadStage: cast not found for slot: " << slot.castId << std::endl;
            continue;
        }

        int idx = slot.podiumIndex;
        if (idx < 0) idx = 0;
        idx = idx % N;

        const float angle = step * static_cast<float>(idx) + ring.yawOffsetRad;
        const float x = cx + charRadius * std::sinf(angle);
        const float z = cz + charRadius * std::cosf(angle);

        RenderBridge::TextureRef charTex{};
        charTex = catalog_->GetOrLoad(device, cast->cardImage);
        if (!charTex.valid) {
            // TODO: log warning about missing character card texture
            // missing texture is set in catalog automatically
            std::cout << "StageWorld::LoadStage: character card texture not found for cast: " << cast->id << std::endl;
            continue;
        }

        Render::CardDrawItem charItem{};
        charItem.srv = charTex.srv;
        charItem.pos = {x, cy, z};
        charItem.yaw = angle; // face outward

        float aspect = charTex.w > 0 ?
            static_cast<float>(charTex.w) /
            static_cast<float>(charTex.h) : 1.0f;
        charItem.size = {charWorldH * aspect, charWorldH};
        charItem.alphaCut = 0.5f;

        cards_.push_back(charItem);

        Anchor anchor{};
        anchor.head = {x, cy + cast->headY, z};
        anchors_[cast->id] = anchor;
    }
}

void StageWorld::EmitBackground(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) const {
    if (!bgTex_.valid || !bgTex_.srv) return;

    float x, y, w, h;
    FitToHeight(canvasW, canvasH, bgTex_.w, bgTex_.h, x, y, w, h);

    drawList.PushSprite(Render::Layer::Background, bgTex_.srv, {x, y, w, h});
}

} // namespace Salt2D::Game::Director
