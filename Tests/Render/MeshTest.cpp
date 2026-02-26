// Tests/Render/MeshTest.cpp
// A standalone test program for loading and rendering meshes with a free camera

#include "Core/Window/Win32Window.h"
#include "Core/Time/FrameClock.h"
#include "Render/DX11Renderer.h"
#include "Resources/Mesh/MeshLoader.h"
#include "Render/Scene3D/MeshFactory.h"
#include "Render/Scene3D/Mesh.h"
#include "Render/Draw/MeshDrawItem.h"
#include "Render/Passes/MeshPass.h"
#include "Render/Passes/ComposePass.h"
#include "Render/RenderPlan.h"

#include <Windows.h>
#include <objbase.h>
#include <imm.h>
#include <DirectXMath.h>
#include <iostream>
#include <memory>
#include <vector>

#pragma comment(lib, "imm32.lib")

using namespace Salt2D;
using namespace DirectX;

// ========================================
// Free Camera Implementation
// ========================================
class FreeCamera {
public:
    FreeCamera() {
        SetPosition(0.0f, 2.0f, -5.0f);
        SetYawPitch(0.0f, 0.0f);
    }

    void SetPosition(float x, float y, float z) {
        position_ = XMFLOAT3{x, y, z};
    }

    void SetYawPitch(float yaw, float pitch) {
        yaw_ = yaw;
        pitch_ = pitch;
        UpdateRotation();
    }

    void SetCaptured(bool captured) {
        captured_ = captured;
    }

    bool IsCaptured() const {
        return captured_;
    }

    void Update(const Core::InputState& input, float dt) {
        // Mouse rotation (only when captured)
        if (captured_) {
            const float mouseSensitivity = 0.001f;
            yaw_ += input.mouseDX * mouseSensitivity;
            pitch_ += input.mouseDY * mouseSensitivity;  // Flipped vertical
            
            // Clamp pitch to avoid gimbal lock
            const float maxPitch = XM_PIDIV2 - 0.01f;
            pitch_ = (std::max)(-maxPitch, (std::min)(maxPitch, pitch_));
            
            UpdateRotation();
        }

        // WASD movement - horizontal plane only (fixed height)
        XMVECTOR horizontalMovement = XMVectorZero();
        const float moveSpeed = input.Down(VK_SHIFT) ? 10.0f : 3.0f;
        
        if (input.Down(Core::Key::W)) horizontalMovement = XMVectorAdd(horizontalMovement, XMVectorSet(0, 0, 1, 0));
        if (input.Down(Core::Key::S)) horizontalMovement = XMVectorAdd(horizontalMovement, XMVectorSet(0, 0, -1, 0));
        if (input.Down(Core::Key::A)) horizontalMovement = XMVectorAdd(horizontalMovement, XMVectorSet(-1, 0, 0, 0));
        if (input.Down(Core::Key::D)) horizontalMovement = XMVectorAdd(horizontalMovement, XMVectorSet(1, 0, 0, 0));
        
        // Apply horizontal movement (only use yaw, ignore pitch)
        if (!XMVector3Equal(horizontalMovement, XMVectorZero())) {
            horizontalMovement = XMVector3Normalize(horizontalMovement);
            horizontalMovement = XMVectorScale(horizontalMovement, moveSpeed * dt);
            
            // Create rotation only around Y-axis (yaw only)
            XMVECTOR yawQuat = XMQuaternionRotationRollPitchYaw(0.0f, yaw_, 0.0f);
            XMVECTOR worldMovement = XMVector3Rotate(horizontalMovement, yawQuat);
            
            XMVECTOR pos = XMLoadFloat3(&position_);
            pos = XMVectorAdd(pos, worldMovement);
            XMStoreFloat3(&position_, pos);
        }
        
        // QE for up/down (vertical movement)
        float verticalMovement = 0.0f;
        if (input.Down(Core::Key::E)) verticalMovement += moveSpeed * dt;
        if (input.Down(Core::Key::Q)) verticalMovement -= moveSpeed * dt;
        
        if (verticalMovement != 0.0f) {
            position_.y += verticalMovement;
        }
    }

    XMMATRIX GetViewMatrix() const {
        XMVECTOR pos = XMLoadFloat3(&position_);
        XMVECTOR rot = XMLoadFloat4(&rotation_);
        
        XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rot);
        XMVECTOR up = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rot);
        XMVECTOR target = XMVectorAdd(pos, forward);
        
        return XMMatrixLookAtLH(pos, target, up);
    }

    XMMATRIX GetProjMatrix(uint32_t width, uint32_t height) const {
        float aspect = static_cast<float>(width) / static_cast<float>(height);
        return XMMatrixPerspectiveFovLH(fovY_, aspect, nearZ_, farZ_);
    }

    const XMFLOAT3& GetPosition() const { return position_; }
    float GetYaw() const { return yaw_; }
    float GetPitch() const { return pitch_; }

    void SetFovY(float fovY) { fovY_ = fovY; }
    void SetClip(float nearZ, float farZ) { nearZ_ = nearZ; farZ_ = farZ; }

private:
    void UpdateRotation() {
        XMVECTOR quat = XMQuaternionRotationRollPitchYaw(pitch_, yaw_, 0.0f);
        XMStoreFloat4(&rotation_, quat);
    }

private:
    XMFLOAT3 position_{0.0f, 0.0f, 0.0f};
    XMFLOAT4 rotation_{0.0f, 0.0f, 0.0f, 1.0f}; // Quaternion
    
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    
    float fovY_ = XM_PIDIV4;  // 45 degrees
    float nearZ_ = 0.1f;
    float farZ_ = 100.0f;
    
    bool captured_ = false;
};

// ========================================
// Mesh Test Application
// ========================================
class MeshTestApp {
public:
    MeshTestApp(const std::string& meshPath) {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        window_ = std::make_unique<Core::Win32Window>("Mesh Test - Free Camera", 1600, 900);
        
        uint32_t canvasW, canvasH;
        window_->GetClientSize(canvasW, canvasH);
        window_->setOnResized([this](uint32_t w, uint32_t h) { OnResized(w, h); });

        renderer_ = std::make_unique<Render::DX11Renderer>(window_->GetHWND(), canvasW, canvasH);
        
        LoadMesh(meshPath);
        
        camera_.SetPosition(0.0f, 2.0f, -8.0f);
        camera_.SetFovY(60.0f * XM_PI / 180.0f);
        camera_.SetClip(0.1f, 100.0f);
        
        std::cout << "=== Mesh Test Controls ===\n";
        std::cout << "Right MB - Toggle camera capture\n";
        std::cout << "WASD     - Move camera (horizontal plane)\n";
        std::cout << "Q/E      - Move down/up\n";
        std::cout << "Mouse    - Rotate camera (when captured)\n";
        std::cout << "Shift    - Move faster\n";
        std::cout << "ESC      - Exit capture / Exit program\n";
        std::cout << "==========================\n\n";
    }

    ~MeshTestApp() {
        mesh_.reset();
        renderer_.reset();
        window_.reset();
        CoUninitialize();
    }

    void LoadMesh(const std::string& meshPath) {
        auto& device = renderer_->Device();

        std::cout << "Loading mesh: " << meshPath << "\n";
        
        Resources::MeshLoadOptions options;
        options.scale = 1.0f;
        options.centerAndNormalize = false;
        
        auto meshData = Resources::MeshLoader::LoadOBJ(meshPath, options);
        if (meshData) {
            mesh_ = std::make_unique<Render::Scene3D::Mesh>(
                Render::Scene3D::MeshFactory::CreateMesh(device, *meshData)
            );
            std::cout << "✓ Loaded successfully: " << meshData->vertices.size() 
                      << " vertices, " << meshData->indices.size() / 3 << " triangles\n\n";
        } else {
            throw std::runtime_error("Failed to load mesh: " + meshPath);
        }
    }

    void OnResized(uint32_t w, uint32_t h) {
        if (w == 0 || h == 0 || w > 32768 || h > 32768) return;
        renderer_->Resize(w, h);
    }

    void Update(const Core::FrameTime& ft, const Core::InputState& input) {
        // Save total time for animations
        totalTime_ = ft.totalSec;
        
        // Toggle camera capture on Right Mouse Button
        if (input.Pressed(VK_RBUTTON)) {
            ToggleCameraCapture();
        }

        // ESC behavior: exit capture if captured, otherwise exit program
        if (input.Pressed(VK_ESCAPE)) {
            if (camera_.IsCaptured()) {
                SetCameraCapture(false);
            } else {
                PostQuitMessage(0);
                return;
            }
        }

        // Update camera
        camera_.Update(input, static_cast<float>(ft.dtSec));

        // Print camera info periodically
        static double lastPrintTime = 0.0;
        if (ft.totalSec - lastPrintTime > 2.0) {
            lastPrintTime = ft.totalSec;
            auto pos = camera_.GetPosition();
            std::cout << "Camera - Pos: (" << pos.x << ", " << pos.y << ", " << pos.z << ") "
                      << "Yaw: " << (camera_.GetYaw() * 180.0f / XM_PI) << "° "
                      << "Pitch: " << (camera_.GetPitch() * 180.0f / XM_PI) << "°\n";
        }
    }

    void Render() {
        if (!mesh_) return;

        uint32_t sceneW = renderer_->GetSceneW();
        uint32_t sceneH = renderer_->GetSceneH();

        // Setup frame blackboard
        Render::FrameBlackboard frame;
        frame.view = camera_.GetViewMatrix();
        frame.proj = camera_.GetProjMatrix(sceneW, sceneH);
        frame.viewProj = frame.view * frame.proj;
        
        // Circular orbiting light (radius 5, height 10, period ~6.28 seconds)
        float t = static_cast<float>(totalTime_);
        frame.lightPosition = {
            2.0f * cosf(t),
            3.0f,
            2.0f * sinf(t)
        };

        // Build mesh draw item
        std::vector<Render::MeshDrawItem> meshItems;
        Render::MeshDrawItem item;
        item.mesh = mesh_.get();
        item.world = XMMatrixIdentity();
        meshItems.push_back(item);

        // Build render plan
        Render::RenderPlan plan;
        plan.Clear();

        // Scene 3D pass with depth testing
        auto meshPass = std::make_unique<Render::MeshPass>(
            "Scene_3D", 
            Render::Target::Scene, 
            Render::DepthMode::RW, 
            Render::BlendMode::Off, 
            meshItems
        );
        meshPass->SetClearScene(0.15f, 0.15f, 0.2f, 1.0f);
        meshPass->SetClearDepth(1.0f, 0);
        plan.passes.push_back(std::move(meshPass));

        // Compose to backbuffer
        auto composePass = std::make_unique<Render::ComposePass>("Compose");
        plan.passes.push_back(std::move(composePass));

        // Execute and present
        renderer_->ExecutePlan(plan, frame);
        renderer_->Present(true);
    }

    void Run() {
        clock_.Reset();

        while (window_->ProcessMessages()) {
            auto& input = window_->GetInputState();
            auto ft = clock_.Tick();

            Update(ft, input);
            
            // Reset mouse to center when captured (after update)
            if (camera_.IsCaptured()) {
                ResetMouseToCenter();
            }
            
            Render();
        }
    }

    void ToggleCameraCapture() {
        SetCameraCapture(!camera_.IsCaptured());
    }

    void SetCameraCapture(bool capture) {
        camera_.SetCaptured(capture);
        
        if (capture) {
            // Hide cursor and reset to center
            ShowCursor(FALSE);
            ResetMouseToCenter();
            
            std::cout << ">>> Camera captured (press ESC to release)\n";
        } else {
            // Show cursor
            ShowCursor(TRUE);
            std::cout << ">>> Camera released (press Right MB to capture)\n";
        }
    }

    void ResetMouseToCenter() {
        RECT rect;
        GetClientRect(window_->GetHWND(), &rect);
        
        // Calculate center in client coordinates
        int centerX = (rect.right - rect.left) / 2;
        int centerY = (rect.bottom - rect.top) / 2;
        
        // Update input state's mouse position to center BEFORE SetCursorPos
        // This prevents the WM_MOUSEMOVE from SetCursorPos from generating delta
        auto& input = window_->GetInputState();
        input.mouseX = centerX;
        input.mouseY = centerY;
        
        // Convert to screen coordinates
        POINT center = {centerX, centerY};
        ClientToScreen(window_->GetHWND(), &center);
        
        // Set cursor position (will generate WM_MOUSEMOVE, but delta will be 0)
        SetCursorPos(center.x, center.y);
    }

private:
    std::unique_ptr<Core::Win32Window> window_;
    std::unique_ptr<Render::DX11Renderer> renderer_;
    Core::FrameClock clock_;
    
    FreeCamera camera_;
    std::unique_ptr<Render::Scene3D::Mesh> mesh_;
    
    double totalTime_ = 0.0;  // Running total time for animations
};

// ========================================
// Main Entry Point
// ========================================
int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    
    // Disable IME (Input Method Editor) for the entire process
    ImmDisableIME(static_cast<DWORD>(-1));
    
    try {
        std::cout << "Starting Mesh Test Program...\n\n";
        
        // Get mesh path from command line or use default
        std::string meshPath = "Assets/Mesh/inquisition.obj";
        if (argc > 1) {
            meshPath = argv[1];
        } else {
            std::cout << "Usage: MeshTest.exe <mesh_path>\n";
            std::cout << "Using default mesh: " << meshPath << "\n\n";
        }
        
        MeshTestApp app(meshPath);
        app.Run();
        
        std::cout << "\nMesh Test completed successfully.\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
}
