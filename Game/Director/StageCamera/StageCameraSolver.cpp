// Game/Director/StageCamera/StageCameraSolver.cpp
#include "StageCameraSolver.h"
#include "Utils/MathUtils.h"

#include <cmath>
#include <algorithm>
#include <iostream>

using namespace DirectX;

namespace Salt2D::Game::Director {

namespace {

static inline XMFLOAT3 NormalizeXZ(const XMFLOAT3& v) {
    float len = std::sqrt(v.x * v.x + v.z * v.z);
    if (len < 1e-6f) return {0, 0, 1};
    return {v.x / len, 0, v.z / len};
}

static inline XMFLOAT3 ComputeRightFromFace(const XMFLOAT3& face) {
    XMVECTOR up = XMVectorSet(0, 1, 0, 0);
    XMVECTOR f  = XMLoadFloat3(&face);
    XMVECTOR r  = XMVector3Cross(up, f);
    XMFLOAT3 right;
    XMStoreFloat3(&right, r);
    return NormalizeXZ(right);
}

static inline XMFLOAT4 EvalFixedQuat(
    const Anchor& anchor,
    float yawRad, float pitchRad, float rollRad
) {
    // Base direction: looking towards the stage (opposite of anchor.face)
    // This gives a consistent world-space orientation regardless of camera position
    XMFLOAT3 baseLookDir = {-anchor.face.x, 0.0f, -anchor.face.z};
    baseLookDir = NormalizeXZ(baseLookDir);
    
    XMVECTOR lookDir = XMLoadFloat3(&baseLookDir);
    XMVECTOR worldUp = XMVectorSet(0, 1, 0, 0);
    
    // Apply yaw rotation around world Y axis
    XMMATRIX yawMat = XMMatrixRotationAxis(worldUp, yawRad);
    XMVECTOR lookDirYaw = XMVector3TransformNormal(lookDir, yawMat);
    
    // Compute right vector for the yaw-rotated look direction
    XMVECTOR rightYaw = XMVector3Normalize(XMVector3Cross(worldUp, lookDirYaw));
    
    // Apply pitch rotation around the right axis
    XMMATRIX pitchMat = XMMatrixRotationAxis(rightYaw, pitchRad);
    XMVECTOR lookDirFinal = XMVector3TransformNormal(lookDirYaw, pitchMat);
    
    // Recompute right and up for the final look direction
    XMVECTOR rightFinal = XMVector3Normalize(XMVector3Cross(worldUp, lookDirFinal));
    XMVECTOR upFinal = XMVector3Cross(lookDirFinal, rightFinal);
    
    // Apply roll rotation around the final look direction
    XMMATRIX rollMat = XMMatrixRotationAxis(lookDirFinal, rollRad);
    XMVECTOR rightRolled = XMVector3TransformNormal(rightFinal, rollMat);
    XMVECTOR upRolled = XMVector3TransformNormal(upFinal, rollMat);
    
    // Build final rotation matrix
    XMMATRIX finalMat = XMMatrixIdentity();
    finalMat.r[0] = rightRolled;
    finalMat.r[1] = upRolled;
    finalMat.r[2] = lookDirFinal;
    
    XMVECTOR qFinal = XMQuaternionRotationMatrix(finalMat);
    qFinal = XMQuaternionNormalize(qFinal);

    XMFLOAT4 finalQuat{};
    XMStoreFloat4(&finalQuat, qFinal);
    return finalQuat;
}

} // Anonymous namespace

void StageCameraSolver::DefaultCamera(const Anchor& anchor, StageCameraSample& out) {
    const XMFLOAT3 up{0, 1, 0};
    const XMFLOAT3 face = anchor.face;

    out.eye = {
        anchor.head.x + face.x * 2.0f + up.x * 0.15f,
        anchor.head.y                 + up.y * 0.15f,
        anchor.head.z + face.z * 2.0f + up.z * 0.15f
    };

    out.target = {
        anchor.head.x,
        anchor.head.y,
        anchor.head.z,
    };

    out.fovYRad = DirectX::XM_PI / 3.0f; // 60 degrees
}

bool StageCameraSolver::EvalAnchorTrackLinear(
    const Story::StageCameraTrack& track,
    const Anchor& anchor, float u01, StageCameraSample& out
) {
    if (track.space != Story::CameraSpace::Anchor) return false; // unsupported space

    const float t = Utils::Clamp01(u01);

    const float dist   = Utils::Lerp(track.start.dist,    track.end.dist,    t);
    const float liftY  = Utils::Lerp(track.start.liftY,   track.end.liftY,   t);
    const float sideX  = Utils::Lerp(track.start.sideX,   track.end.sideX,   t);
    const float fovDeg = Utils::Lerp(track.start.fovYDeg, track.end.fovYDeg, t);

    const XMFLOAT3 up{0, 1, 0};
    const XMFLOAT3 face = anchor.face;
    const XMFLOAT3 side = ComputeRightFromFace(face);

    out.eye = {
        anchor.head.x + face.x * dist + up.x * liftY + side.x * sideX,
        anchor.head.y                 + up.y * liftY,
        anchor.head.z + face.z * dist + up.z * liftY + side.z * sideX
    };

    out.target = {
        anchor.head.x,
        anchor.head.y + track.targetListY,
        anchor.head.z,
    };

    out.fovYRad = Utils::DegToRad(fovDeg);
    return true;
}

StageCameraRotationResult StageCameraSolver::EvalRotation(
    const Story::Rotation3DPolicy& policy,
    const Anchor& anchor, const StageCameraSample& 
) {
    StageCameraRotationResult result{};

    switch (policy.mode) {
    case Story::Rotate3DMode::Fixed:
        result.useFixed = true;
        result.quat = EvalFixedQuat(anchor,
            policy.fixedYawRad,
            policy.fixedPitchRad,
            policy.fixedRollRad);
        return result;
    case Story::Rotate3DMode::LookAt:
    default:
        result.useFixed = false;
        return result;
    }
}

} // namespace Salt2D::Game::Director