// Resources/Mesh/MeshLoader.h
#ifndef RESOURCES_MESH_MESHLOADER_H
#define RESOURCES_MESH_MESHLOADER_H

#include <memory>
#include <DirectXMath.h>

#include "Utils/FileUtils.h"
#include "Resources/Mesh/MeshData.h"

namespace Salt2D::Resources {

struct MeshLoadOptions {
    bool triangulate = true;
    bool flipWinding = false;
    bool flipZ = false;
    bool flipV = true;
    float scale = 1.0f;

    bool centerAndNormalize = false;
    DirectX::XMFLOAT3 defaultColor = { 0.9f, 0.9f, 0.9f };
};

class MeshLoader {
public:
    static std::shared_ptr<MeshData> LoadOBJ(
        const std::filesystem::path& path,
        const MeshLoadOptions& options = {}
    );
};

} // namespace Salt2D::Resources

#endif // RESOURCES_MESH_MESHLOADER_H