// Render/Demo/CubeDemo.cpp
#include "CubeDemo.h"
#include "Render/Pipelines/MeshPipeline.h"
#include "Render/Drawers/MeshDrawer.h"
#include "Render/RenderPlan.h"
#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11Common.h"

#include <vector>
#include <stdexcept>

using namespace DirectX;

namespace Salt2D::Render {

namespace {

static void BuildCube(std::vector<MeshPipeline::VertexCPU>& vtx, std::vector<uint16_t>& idx) {
    const float s = 0.5f;

    auto C = [](float r, float g, float b) { return XMFLOAT4(r, g, b, 1.0f); };

    struct Face { XMFLOAT3 n; XMFLOAT4 c; XMFLOAT3 v[4]; };

    Face faces[] = {
        { {+1,0,0}, C(1,0,0), { {+s,-s,-s}, {+s,+s,-s}, {+s,+s,+s}, {+s,-s,+s} } }, // +X 
        { {-1,0,0}, C(0,1,0), { {-s,-s,+s}, {-s,+s,+s}, {-s,+s,-s}, {-s,-s,-s} } }, // -X 
        { {0,+1,0}, C(0,0,1), { {-s,+s,+s}, {+s,+s,+s}, {+s,+s,-s}, {-s,+s,-s} } }, // +Y
        { {0,-1,0}, C(1,1,0), { {-s,-s,-s}, {+s,-s,-s}, {+s,-s,+s}, {-s,-s,+s} } }, // -Y
        { {0,0,+1}, C(1,0,1), { {-s,+s,+s}, {-s,-s,+s}, {+s,-s,+s}, {+s,+s,+s} } }, // +Z
        { {0,0,-1}, C(0,1,1), { {+s,+s,-s}, {+s,-s,-s}, {-s,-s,-s}, {-s,+s,-s} } }, // -Z
    };

    vtx.clear();
    idx.clear();
    vtx.reserve(24);
    idx.reserve(36);

    for (const auto& face : faces) {
        uint16_t baseIndex = static_cast<uint16_t>(vtx.size());
        for (int i = 0; i < 4; ++i) {
            MeshPipeline::VertexCPU vertex;
            vertex.pos = face.v[i];
            vertex.color = face.c;
            vtx.push_back(vertex);
        }
        idx.push_back(baseIndex + 0);
        idx.push_back(baseIndex + 1);
        idx.push_back(baseIndex + 2);
        idx.push_back(baseIndex + 0);
        idx.push_back(baseIndex + 2);
        idx.push_back(baseIndex + 3);
    }
}

} // anonymous namespace

void CubeDemo::Initialize(const RHI::DX11::DX11Device& device) {
    auto d3dDevice = device.GetDevice();

    std::vector<MeshPipeline::VertexCPU> vertices;
    std::vector<uint16_t> indices;
    BuildCube(vertices, indices);
    indexCount_ = static_cast<uint32_t>(indices.size());

    D3D11_BUFFER_DESC vbDesc{};
    vbDesc.ByteWidth = static_cast<UINT>(sizeof(MeshPipeline::VertexCPU) * vertices.size());
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData{};
    vbData.pSysMem = vertices.data();

    ThrowIfFailed(d3dDevice->CreateBuffer(&vbDesc, &vbData, vb_.GetAddressOf()),
        "CubeDemo::Initialize: Failed to create vertex buffer.");

    D3D11_BUFFER_DESC ibDesc{};
    ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint16_t) * indices.size());
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData{};
    ibData.pSysMem = indices.data();

    ThrowIfFailed(d3dDevice->CreateBuffer(&ibDesc, &ibData, ib_.GetAddressOf()),
        "CubeDemo::Initialize: Failed to create index buffer.");
}

void CubeDemo::Draw(PassContext& ctx, const DirectX::XMMATRIX& worldViewProj) {
    ctx.draw->Mesh().DrawMesh(ctx, vb_.Get(), ib_.Get(), indexCount_, worldViewProj);
}

} // namespace Salt2D::Render
