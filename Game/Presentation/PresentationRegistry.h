// Game/Presentation/PresentationRegistry.h
#ifndef GAME_PRESENTATION_PRESENTATIONREGISTRY_H
#define GAME_PRESENTATION_PRESENTATIONREGISTRY_H

#include <array>
#include <memory>
#include <cstddef>
#include <cstdint>

#include "IChapterPresentation.h"
#include "Game/Flow/GameFlowTypes.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Game::Story {
    class StoryPlayer;
} // namespace Salt2D::Game::Story

namespace Salt2D::Game::Session {
    class StorySession;
} // namespace Salt2D::Game::Session

namespace Salt2D::Game::RenderBridge {
    class TextureCatalog;
} // namespace Salt2D::Game::RenderBridge

namespace Salt2D::Game::Presentation {

enum class PresKey : uint8_t {
    Novel = 0,
    Trial = 1,
    Null  = 2,
    Count
};

inline constexpr size_t ToIndex(PresKey key) {
    return static_cast<size_t>(key);
}

inline constexpr PresKey ToKey(Flow::ChapterKind kind) {
    switch (kind) {
    case Flow::ChapterKind::Novel: return PresKey::Novel;
    case Flow::ChapterKind::Trial: return PresKey::Trial;
    default: return PresKey::Null; // Invalid
    }
}

class PresentationRegistry {
public:
    PresentationRegistry() = default;
    PresentationRegistry(const PresentationRegistry&) = delete;
    PresentationRegistry& operator=(const PresentationRegistry&) = delete;

    ~PresentationRegistry();

    void Initialize(RenderBridge::TextureCatalog& catalog);

    void OnSessionCleared();
    void OnFinished();
    void OnSessionCreated(const RHI::DX11::DX11Device& device,
        const Flow::ChapterDef& chapter, Session::StorySession& session);

    void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft);
    void EmitSceneDraw(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH);
    void BuildScenePasses(Render::RenderPlan& plan, const Render::DrawList& drawList);
    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t canvasW, uint32_t canvasH);

private:
    void SetActive(PresKey key);
    IChapterPresentation& Active();

private:
    using Slot = std::unique_ptr<IChapterPresentation>;
    std::array<Slot, static_cast<size_t>(PresKey::Count)> slots_{};

    PresKey activeKey_ = PresKey::Null;
    bool bound_ = false;
};

} // namespace Salt2D::Game::Presentation

#endif // GAME_PRESENTATION_PRESENTATIONREGISTRY_H
