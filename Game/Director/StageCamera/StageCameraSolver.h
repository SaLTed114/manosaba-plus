// Game/Director/StageCamera/StageCameraSolver.h
#ifndef GAME_DIRECTOR_STAGECAMERA_STAGECAMERASOLVER_H
#define GAME_DIRECTOR_STAGECAMERA_STAGECAMERASOLVER_H

#include "Game/Director/StageCamera/StageCameraTypes.h"
#include "Game/Director/StageWorld.h"
#include "Game/Story/Resources/PerformanceDef.h"

namespace Salt2D::Game::Director {

class StageCameraSolver {
public:
    static void DefaultCamera(const Anchor& anchor, StageCameraSample& out);

    static bool EvalAnchorTrackLinear(
        const Story::StageCameraTrack& track,
        const Anchor& anchor, float u01, StageCameraSample& out);

    static StageCameraRotationResult EvalRotation(
        const Story::Rotation3DPolicy& policy,
        const Anchor& anchor, const StageCameraSample& sample);
};

};

#endif // GAME_DIRECTOR_STAGECAMERA_STAGECAMERASOLVER_H
