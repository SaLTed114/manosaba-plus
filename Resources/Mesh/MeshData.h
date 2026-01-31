// Resources/Mesh/MeshData.h
#ifndef RESOURCES_MESH_MESHDATA_H
#define RESOURCES_MESH_MESHDATA_H

#include <vector>
#include <cstdint>
#include <DirectXMath.h>

namespace Salt2D::Resources {

struct Vertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;
    DirectX::XMFLOAT3 color;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

} // namespace Salt2D::Resources

#endif // RESOURCES_MESH_MESHDATA_H
