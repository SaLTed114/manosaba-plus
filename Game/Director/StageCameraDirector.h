// Game/Director/StageCameraDirector.h
#ifndef GAME_DIRECTOR_STAGECAMERADIRECTOR_H
#define GAME_DIRECTOR_STAGECAMERADIRECTOR_H

#include "StageWorld.h"
#include "Game/Story/StoryTables.h"
#include "Game/Story/StoryPlayer.h"
#include "Render/Scene3D/Camera3D.h"

namespace Salt2D::Game::Director {

struct CameraDirectorConfig {
    float dist = 2.0f;
    float liftY = 0.15f;
};

class StageCameraDirector {
public:
    void Initialize(StageWorld* world,
        const Story::StoryTables* tables,
        Render::Scene3D::Camera3D* camera);

    void SetConfig(const CameraDirectorConfig& cfg) { cfg_ = cfg; }

    void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft);

private:
    CameraDirectorConfig cfg_{};

    StageWorld* world_ = nullptr;
    const Story::StoryTables* tables_ = nullptr;
    Render::Scene3D::Camera3D* camera_ = nullptr;
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_STAGECAMERADIRECTOR_H
