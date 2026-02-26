// Game/Flow/GameFlowTypes.h
#ifndef GAME_FLOW_GAMEFLOWTYPES_H
#define GAME_FLOW_GAMEFLOWTYPES_H

#include <string>
#include <filesystem>

namespace Salt2D::Game::Flow {

enum class ChapterKind { Novel, Trial };

struct ChapterDef {
    std::string chapterId;
    ChapterKind kind = ChapterKind::Novel;

    std::filesystem::path storyRoot;
    std::filesystem::path graphPath;
    std::string startNode;
    std::string stageId;
};

enum class FlowState {
    Idle,
    InChapter,
    Finished,
    GateReady,
};

enum class FlowEvent {
    None,
    SessionCleared,
    SessionCreated,
    Finished,
};

} // namespace Salt2D::Game::Flow

#endif // GAME_FLOW_GAMEFLOWTYPES_H
