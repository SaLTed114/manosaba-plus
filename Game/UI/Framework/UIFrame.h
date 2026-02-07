// Game/UI/Framework/UIFrame.h
#ifndef GAME_UI_FRAMEWORK_UIFRAME_H
#define GAME_UI_FRAMEWORK_UIFRAME_H

#include <vector>
#include <string>
#include <cstdint>
#include "Render/Draw/SpriteDrawItem.h"
#include "Render/Text/TextBaker.h"
#include "Game/UI/UITypes.h"

struct ID3D11ShaderResourceView;

namespace Salt2D::Game::UI {

enum class HitAction : uint8_t {
    None = 0,
    Next,
};

struct SpriteOp {
    Render::Layer layer = Render::Layer::HUD;

    TextureId texId = TextureId::White;
    ID3D11ShaderResourceView* srv = nullptr; // optional override
    
    Render::RectF dst{};
    Render::UVRectF uv{};
    Render::Color4F tint{1,1,1,1};
    float z = 0.0f;

    bool clipEnabled = false;
    Render::RectI clipRect{};
};

struct TextOp {
    Render::Layer layer = Render::Layer::HUD;
    TextStyleId styleId = TextStyleId::VnBody;

    std::string textUtf8;
    float layoutW = 1.0f;
    float layoutH = 1.0f;

    float x = 0.0f;
    float y = 0.0f;

    Render::Color4F tint{1,1,1,1};
    float z = 0.0f;

    Render::Text::BakedText baked{};
};

struct HitOp {
    HitKey key = 0;
    Render::RectF rect{};
    HitAction action = HitAction::None;
    bool enabled = true;
    bool visible = true;
};

struct UIFrame {
    std::vector<SpriteOp> sprites;
    std::vector<TextOp>   texts;
    std::vector<HitOp>    hits;

    void Clear() {
        sprites.clear();
        texts.clear();
        hits.clear();
    }
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIFRAME_H
