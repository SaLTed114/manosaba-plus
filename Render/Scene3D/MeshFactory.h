// Render/Scene3D/MeshFactory.h
#pragma once

#include "Mesh.h"
#include "Resources/Mesh/MeshData.h"

namespace Salt2D::RHI::DX11 {
class DX11Device;
}

namespace Salt2D::Render::Scene3D {

class MeshFactory {
public:
    static Mesh CreateMesh(
        const RHI::DX11::DX11Device& device,
        const Resources::MeshData& meshData
    );
};

} // namespace Salt2D::Render::Scene3D
