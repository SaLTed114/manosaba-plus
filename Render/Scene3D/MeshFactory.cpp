// Render/Scene3D/MeshFactory.cpp
#include "MeshFactory.h"
#include "Render/Pipelines/MeshPipeline.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11Common.h"

#include <stdexcept>

namespace Salt2D::Render::Scene3D {

Mesh MeshFactory::CreateMesh(
    const RHI::DX11::DX11Device& device,
    const Resources::MeshData& meshData
) {
    if (meshData.vertices.empty() || meshData.indices.empty()) {
        throw std::runtime_error("MeshFactory: Cannot create mesh from empty data");
    }

    // Convert Vertex to MeshPipeline::VertexCPU
    std::vector<MeshPipeline::VertexCPU> vertices;
    vertices.reserve(meshData.vertices.size());

    for (const auto& v : meshData.vertices) {
        MeshPipeline::VertexCPU vertex;
        vertex.pos = v.position;
        vertex.normal = v.normal;
        vertex.uv = v.texCoord;
        vertex.color = DirectX::XMFLOAT4(v.color.x, v.color.y, v.color.z, 1.0f);
        vertices.push_back(vertex);
    }

    auto d3dDevice = device.GetDevice();
    Mesh mesh;

    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(MeshPipeline::VertexCPU) * vertices.size());
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();

    ThrowIfFailed(d3dDevice->CreateBuffer(&vbDesc, &vbData, mesh.vb.GetAddressOf()),
        "MeshFactory: Failed to create vertex buffer");

    // Create index buffer
    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * meshData.indices.size());
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = meshData.indices.data();

    ThrowIfFailed(d3dDevice->CreateBuffer(&ibDesc, &ibData, mesh.ib.GetAddressOf()),
        "MeshFactory: Failed to create index buffer");

    // Set mesh properties
    mesh.stride = sizeof(MeshPipeline::VertexCPU);
    mesh.indexFormat = DXGI_FORMAT_R32_UINT;
    mesh.indexCount = static_cast<uint32_t>(meshData.indices.size());

    return mesh;
}

} // namespace Salt2D::Render::Scene3D
