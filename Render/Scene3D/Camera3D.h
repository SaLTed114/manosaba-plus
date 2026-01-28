// Render/Scene3D/Camera3D.h
#ifndef RENDER_SCENE3D_CAMERA3D_H
#define RENDER_SCENE3D_CAMERA3D_H

#include <cstdint>
#include <DirectXMath.h>

namespace Salt2D::Render::Scene3D {

class Camera3D {
public:
    void SetPosition(const DirectX::XMFLOAT3& pos) { position_ = pos; }
    void SetPosition(float x, float y, float z) { position_ = DirectX::XMFLOAT3{x, y, z}; }

    void SetRotationQuat(const DirectX::XMFLOAT4& quat);
    void SetRotationQuat(float x, float y, float z, float w);
    void SetYawPitchRoll(float yaw, float pitch, float roll);

    const DirectX::XMFLOAT3& GetPosition() const { return position_; }
    const DirectX::XMFLOAT4& GetRotationQuat() const { return rotation_; }

    void TranslateLocal(const DirectX::XMFLOAT3& delta);
    void TranslateLocal(float dx, float dy, float dz);
    void RotateLocalQuat(const DirectX::XMFLOAT4& deltaQuat);
    void RotateLocalQuat(float dx, float dy, float dz, float dw);
    void RotateLocalYawPitchRoll(float dyaw, float dpitch, float droll);

    DirectX::XMMATRIX GetView() const;
    DirectX::XMMATRIX GetProj(uint32_t width, uint32_t height) const;

    void LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up = {0,1,0});

    void SetClip(float nearZ, float farZ) { nearZ_ = nearZ; farZ_ = farZ; }
    void SetFovY(float fovY) { fovY_ = fovY; }

private:
    DirectX::XMFLOAT3 position_{0.0f, 0.0f, -2.5f};
    DirectX::XMFLOAT4 rotation_{0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion (x, y, z, w)

    float fovY_  = DirectX::XM_PIDIV4; // 45 degrees
    float nearZ_ = 0.1f;
    float farZ_  = 100.0f;
};

} // namespace Salt2D::Render::Scene3D

#endif // RENDER_SCENE3D_CAMERA3D_H
