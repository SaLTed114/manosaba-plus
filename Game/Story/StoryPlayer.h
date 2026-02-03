// Game/Story/StoryPlayer.h
#ifndef GAME_STORY_STORYPLAYER_H
#define GAME_STORY_STORYPLAYER_H

#include <optional>
#include "StoryRuntime.h"
#include "StoryView.h"
#include "Game/Story/Runners/VnRunner.h"
#include "Game/Story/Runners/PresentRunner.h"
#include "Game/Story/Runners/DebateRunner.h"
#include "Game/Common/Logger.h"

namespace Salt2D::Game::Story {

class StoryPlayer {
public:
    StoryPlayer(const StoryGraph& graph, Utils::IFileSystem& fs);

    void Start(const NodeId& startNode);

    void Advance();
    void FastForward();

    void CommitOption(const std::string& optionId);
    void PickEvidence(const std::string& evidenceId);

    void OpenSuspicion(const std::string& spanId);
    void CloseDebateMenu();

    const NodeId& CurrentNodeId() const { return rt_.CurrentNodeId(); }
    const Node&   CurrentNode()   const { return rt_.CurrentNode(); }

    const StoryView& View() const { return view_; }

    void SetEffectCallback(StoryRuntime::EffectCallback callback) { rt_.SetEffectCallback(std::move(callback)); }
    
    void SetLogger(const Game::Logger* logger) { 
        logger_ = logger; 
        rt_.SetLogger(logger); 
        vn_.SetLogger(logger); 
        present_.SetLogger(logger);
        debate_.SetLogger(logger);
    }

private:
    void OnEnteredNode();
    void PumpAuto();
    void UpdateView();

private:
    StoryRuntime rt_;
    Utils::IFileSystem& fs_;

    VnRunner      vn_;
    PresentRunner present_;
    DebateRunner  debate_; 

    StoryView view_;

    const Game::Logger* logger_ = nullptr;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYPLAYER_H
