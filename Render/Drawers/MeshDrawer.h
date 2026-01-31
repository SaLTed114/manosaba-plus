// Render/Drawers/MeshDrawer.h
#ifndef RENDER_DRAWERS_MESHDRAWER_H
#define RENDER_DRAWERS_MESHDRAWER_H

#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Render/Draw/MeshDrawItem.h"
#include "Render/RenderPlan.h"
#include <span>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class MeshDrawer {
public:
    void Initialize(const RHI::DX11::DX11Device& device);

    void DrawBatch(PassContext& ctx, std::span<const MeshDrawItem> meshes);
    void DrawItem(PassContext& ctx, const MeshDrawItem& item);

private:
    void Draw(PassContext& ctx, const MeshDrawItem& item);
};

} // namespace Salt2D::Render

#endif // RENDER_DRAWERS_MESHDRAWER_H
