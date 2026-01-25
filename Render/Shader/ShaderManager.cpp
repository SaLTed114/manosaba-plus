// Render/Shader/ShaderManager.cpp
#include "ShaderManager.h"
#include "RHI/DX11/DX11Common.h"

#include <stdexcept>

using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

namespace Salt2D::Render {

ShaderManager::ShaderManager(std::vector<fs::path> searchDirs)
    : searchDirs_(std::move(searchDirs)) {}

ComPtr<ID3DBlob> ShaderManager::LoadShader(const fs::path& relativePath) {
    const auto key = relativePath.wstring();
    if (auto it = shaderCache_.find(key); it != shaderCache_.end()) {
        return it->second;
    }

    std::vector<fs::path> candidates;
    for (const auto& dir : searchDirs_) {
        fs::path p = dir / relativePath;
        candidates.push_back(p);
        if (fs::exists(p)) {
            ComPtr<ID3DBlob> blob = LoadFromDisk(p);
            shaderCache_[key] = blob;
            return blob;
        }
    }

    std::string msg = "Shader file not found: " + relativePath.string() + "\nSearched paths:\n";
    for (auto& c : candidates) msg += " - " + c.string() + "\n";
    throw std::runtime_error(msg);
}

ComPtr<ID3DBlob> ShaderManager::LoadFromDisk(const fs::path& filepath) {
    ComPtr<ID3DBlob> blob;
    auto errMsg = "Failed to read shader blob: " + filepath.string();
    ThrowIfFailed(D3DReadFileToBlob(filepath.c_str(), blob.GetAddressOf()), errMsg.c_str());
    return blob;
}

void ShaderManager::ClearCache() {
    shaderCache_.clear();
}

} // namespace Salt2D::Render
