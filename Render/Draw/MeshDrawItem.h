// Render/Draw/MeshDrawItem.h
#ifndef RENDER_DRAW_MESHDRAWITEM_H
#define RENDER_DRAW_MESHDRAWITEM_H

#include <DirectXMath.h>
#include "Render/Scene3D/Mesh.h"

namespace Salt2D::Render {

struct MeshDrawItem {
    const Scene3D::Mesh* mesh = nullptr;
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
};

} // namespace Salt2D::Render

#endif // RENDER_DRAW_MESHDRAWITEM_H
