// Render/Drawers/DrawServices.h
#ifndef RENDER_DRAWERS_DRAWSERVICES_H
#define RENDER_DRAWERS_DRAWSERVICES_H

#include <memory>

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {

class SpriteBatcher;
class MeshDrawer;
class CardDrawer;

class DrawServices {
public:
    DrawServices();
    ~DrawServices();

    void Initialize(const RHI::DX11::DX11Device& device);

    SpriteBatcher& Sprite() { return *sprite_; }
    MeshDrawer&    Mesh()   { return *mesh_; }
    CardDrawer&    Card()   { return *card_; }

private:
    std::unique_ptr<SpriteBatcher> sprite_;
    std::unique_ptr<MeshDrawer>    mesh_;
    std::unique_ptr<CardDrawer>    card_;
};

} // namespace Salt2D::Render

#endif // RENDER_DRAWERS_DRAWSERVICES_H
