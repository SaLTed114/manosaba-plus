// Game/Director/StageCameraDirector.h
#ifndef GAME_DIRECTOR_STAGECAMERADIRECTOR_H
#define GAME_DIRECTOR_STAGECAMERADIRECTOR_H

#include "StageWorld.h"
#include "Game/Story/StoryTables.h"
#include "Game/Story/StoryPlayer.h"
#include "Render/Scene3D/Camera3D.h"
#include "Game/Director/StageCamera/StageCameraTypes.h"
#include "Game/Director/StageCamera/StageCameraSolver.h"
#include "Game/Director/SceneCrossfadeController.h"

#include <string>

namespace Salt2D::Game::Director {

class StageCameraDirector {
public:
    void Initialize(Render::Scene3D::Camera3D* camera);
    void Bind(StageWorld* world, const Story::StoryTables* tables);
    void Unbind();
    void Reset();

    void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft);

    float SceneCrossfade() const { return sceneCrossfade_; }
    bool  LockPrevScene() const { return lockPrevScene_; }

private:
    StageWorld* world_ = nullptr;
    const Story::StoryTables* tables_ = nullptr;
    Render::Scene3D::Camera3D* camera_ = nullptr;

    StageCameraSample lastSample_{};
    StageCameraRotationResult lastRotation_{};

    SceneCrossfadeController vnFade_;
    float sceneCrossfade_ = 1.0f;
    bool  lockPrevScene_ = false;
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_STAGECAMERADIRECTOR_H
