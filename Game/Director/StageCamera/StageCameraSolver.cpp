// Game/Director/StageCamera/StageCameraSolver.cpp
#include "StageCameraSolver.h"
#include "Utils/MathUtils.h"

#include <cmath>
#include <algorithm>

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

struct AnchorBasis {
    XMFLOAT3 right{};
    XMFLOAT3 up{0, 1, 0};
    XMFLOAT3 forward{};
};

static inline AnchorBasis BuildAnchorBasis(const Anchor& anchor) {
    // yaw=0: face to anchor
    XMFLOAT3 f0 = {-anchor.face.x, 0, -anchor.face.z};
    f0 = NormalizeXZ(f0);

    const XMFLOAT3 up{0, 1, 0};

    XMVECTOR U = XMLoadFloat3(&up);
    XMVECTOR F = XMLoadFloat3(&f0);
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, F));

    XMFLOAT3 r0{};
    XMStoreFloat3(&r0, R);
    r0 = NormalizeXZ(r0);

    XMVECTOR R2 = XMLoadFloat3(&r0);
    XMVECTOR F2 = XMVector3Normalize(XMVector3Cross(R2, U));
    XMFLOAT3 f1{};
    XMStoreFloat3(&f1, F2);

    AnchorBasis basis{};
    basis.up = up;
    basis.right = r0;
    basis.forward = f1;
    return basis;
}

static inline XMFLOAT4 QuatFromBasis(const AnchorBasis& basis) {
    XMVECTOR R = XMLoadFloat3(&basis.right);
    XMVECTOR U = XMLoadFloat3(&basis.up);
    XMVECTOR F = XMLoadFloat3(&basis.forward);

    XMMATRIX m = XMMatrixIdentity();
    m.r[0] = R;
    m.r[1] = U;
    m.r[2] = F;

    XMVECTOR q = XMQuaternionRotationMatrix(m);
    q = XMQuaternionNormalize(q);
    XMFLOAT4 quat{};
    XMStoreFloat4(&quat, q);
    return quat;
}

static inline XMFLOAT4 EvalFixedQuat(
    const Anchor& anchor,
    float yawRad, float pitchRad, float rollRad
) {
    AnchorBasis basis = BuildAnchorBasis(anchor);

    XMVECTOR up0 = XMLoadFloat3(&basis.up);
    XMVECTOR right0 = XMLoadFloat3(&basis.right);
    XMVECTOR forward0 = XMLoadFloat3(&basis.forward);

    XMVECTOR qYaw   = XMQuaternionRotationAxis(up0, yawRad);

    XMVECTOR right1 = XMVector3Rotate(right0, qYaw);
    XMVECTOR qPitch = XMQuaternionRotationAxis(right1, pitchRad);

    XMVECTOR qYawPitch = XMQuaternionMultiply(qPitch, qYaw);
    XMVECTOR forward2 = XMVector3Rotate(forward0, qYawPitch);
    XMVECTOR qRoll = XMQuaternionRotationAxis(forward2, rollRad);

    XMFLOAT4 baseQuat = QuatFromBasis(basis);
    XMVECTOR qBase = XMLoadFloat4(&baseQuat);

    XMVECTOR qFinal = 
        XMQuaternionMultiply(qRoll,
        XMQuaternionMultiply(qPitch,
        XMQuaternionMultiply(qYaw, qBase)));
    
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
    const Anchor& anchor, const StageCameraSample& /*sample*/
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