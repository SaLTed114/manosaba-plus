// Render/Shader/ShaderManager.h
#ifndef RENDER_SHADER_SHADERMANAGER_H
#define RENDER_SHADER_SHADERMANAGER_H

#include <wrl.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace Salt2D::Render {

class ShaderManager {
public:
    explicit ShaderManager(std::vector<std::filesystem::path> searchDirs);

    Microsoft::WRL::ComPtr<ID3DBlob> LoadShader(const std::filesystem::path& relativePath);
    void ClearCache();

private:
    Microsoft::WRL::ComPtr<ID3DBlob> LoadFromDisk(const std::filesystem::path& filepath);

private:
    std::vector<std::filesystem::path> searchDirs_;
    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID3DBlob>> shaderCache_;
};

} // namespace Salt2D::Render

#endif // RENDER_SHADER_SHADERMANAGER_H
