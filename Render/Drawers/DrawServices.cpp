// Render/Drawers/DrawServices.cpp
#include "DrawServices.h"
#include "SpriteBatcher.h"
#include "MeshDrawer.h"
#include "CardDrawer.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Render {

DrawServices::DrawServices()  = default;
DrawServices::~DrawServices() = default;

void DrawServices::Initialize(const RHI::DX11::DX11Device& device) {
    sprite_ = std::make_unique<SpriteBatcher>();
    sprite_->Initialize(device);

    mesh_ = std::make_unique<MeshDrawer>();
    mesh_->Initialize(device);

    card_ = std::make_unique<CardDrawer>();
    card_->Initialize(device);
}

} // namespace Salt2D::Render
