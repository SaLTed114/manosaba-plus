// Render/Scene3D/Camera3D.cpp
#include "Camera3D.h"

#include <algorithm>

using namespace DirectX;

namespace Salt2D::Render::Scene3D {

// ========================== Rotation ==========================

void Camera3D::SetRotationQuat(const XMFLOAT4& quat) {
    rotation_ = quat;
}

void Camera3D::SetRotationQuat(float x, float y, float z, float w) {
    rotation_ = XMFLOAT4{x, y, z, w};
}

void Camera3D::SetYawPitchRoll(float yaw, float pitch, float roll) {
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&rotation_, q);
}

// ========================== Local Transform ==========================

void Camera3D::TranslateLocal(const XMFLOAT3& delta) {
    TranslateLocal(delta.x, delta.y, delta.z);
}

void Camera3D::TranslateLocal(float dx, float dy, float dz) {
    XMVECTOR pos = XMLoadFloat3(&position_);
    XMVECTOR rot = XMLoadFloat4(&rotation_);
    XMVECTOR localDelta = XMVectorSet(dx, dy, dz, 0.0f);
    
    // Rotate local delta by camera rotation to get world delta
    XMVECTOR worldDelta = XMVector3Rotate(localDelta, rot);
    pos = XMVectorAdd(pos, worldDelta);
    
    XMStoreFloat3(&position_, pos);
}

void Camera3D::RotateLocalQuat(const XMFLOAT4& deltaQuat) {
    RotateLocalQuat(deltaQuat.x, deltaQuat.y, deltaQuat.z, deltaQuat.w);
}

void Camera3D::RotateLocalQuat(float dx, float dy, float dz, float dw) {
    XMVECTOR currentRot = XMLoadFloat4(&rotation_);
    XMVECTOR deltaRot = XMVectorSet(dx, dy, dz, dw);
    
    // Combine rotations: new = current * delta
    XMVECTOR newRot = XMQuaternionMultiply(currentRot, deltaRot);
    newRot = XMQuaternionNormalize(newRot);
    
    XMStoreFloat4(&rotation_, newRot);
}

void Camera3D::RotateLocalYawPitchRoll(float dyaw, float dpitch, float droll) {
    XMVECTOR deltaQuat = XMQuaternionRotationRollPitchYaw(dpitch, dyaw, droll);
    XMFLOAT4 delta;
    XMStoreFloat4(&delta, deltaQuat);
    RotateLocalQuat(delta);
}

// ========================== View/Projection ==========================

XMMATRIX Camera3D::GetView() const {
    XMVECTOR pos = XMLoadFloat3(&position_);
    XMVECTOR rot = XMLoadFloat4(&rotation_);
    
    // Get forward, right, up vectors from quaternion
    XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rot);
    XMVECTOR up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rot);
    
    XMVECTOR target = XMVectorAdd(pos, forward);
    
    return XMMatrixLookAtLH(pos, target, up);
}

XMMATRIX Camera3D::GetProj(uint32_t width, uint32_t height) const {
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    return XMMatrixPerspectiveFovLH(fovY_, aspect, nearZ_, farZ_);
}

void Camera3D::LookAt(const XMFLOAT3& target, const XMFLOAT3& up) {
    XMVECTOR posVec = XMLoadFloat3(&position_);
    XMVECTOR targetVec = XMLoadFloat3(&target);
    XMVECTOR upVec = XMLoadFloat3(&up);
    
    // Calculate forward direction
    XMVECTOR forward = XMVector3Normalize(XMVectorSubtract(targetVec, posVec));
    
    // Calculate right direction
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(upVec, forward));
    
    // Recalculate up to ensure orthogonality
    XMVECTOR newUp = XMVector3Cross(forward, right);
    
    // Build rotation matrix from basis vectors
    XMMATRIX rotMatrix = XMMatrixIdentity();
    rotMatrix.r[0] = XMVectorSetW(right, 0);
    rotMatrix.r[1] = XMVectorSetW(newUp, 0);
    rotMatrix.r[2] = XMVectorSetW(forward, 0);
    
    // Convert rotation matrix to quaternion
    XMVECTOR quat = XMQuaternionRotationMatrix(rotMatrix);
    XMStoreFloat4(&rotation_, quat);
}

} // namespace Salt2D::Render::Scene3D
