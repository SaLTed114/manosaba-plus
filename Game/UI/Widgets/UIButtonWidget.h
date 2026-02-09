// Game/UI/Widgets/UIButtonWidget.h
#ifndef GAME_UI_WIDGETS_UIBUTTONWIDGET_H
#define GAME_UI_WIDGETS_UIBUTTONWIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"

#include <string>
#include <vector>

namespace Salt2D::Game::UI {

struct RectRel { float x=0, y=0, w=1, h=1; };

inline Render::RectF RectIn(const Render::RectF& base, const RectRel& rel) {
    return Render::RectF{
        base.x + rel.x * base.w,
        base.y + rel.y * base.h,
        rel.w * base.w,
        rel.h * base.h
    };
}

inline Render::RectF Inset(const Render::RectF& rect,
    float insetL, float insetT, float insetR, float insetB
) {
    return Render::RectF{
        rect.x + insetL,
        rect.y + insetT,
        rect.w - insetL - insetR,
        rect.h - insetT - insetB
    };
}

struct TintSet {
    Render::Color4F rest{1,1,1,1};
    Render::Color4F hover{1,1,1,1};
    Render::Color4F disabled{1,1,1,1};
};

class UIButtonWidget {
public:
    void Build(UIFrame& frame, HitKey key, Render::RectF rect, bool enabled = true, bool visible = true);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryClick(HitKey clickedKey) const;

    void SetPadding(float l, float t, float r, float b);

    int AddSprite(UIFrame& frame, TextureId texId, RectRel dstRel, TintSet tints,
        float z = 0.0f, Render::Layer layer = Render::Layer::HUD);

    int AddText(UIFrame& frame, TextStyleId styleId, std::string textUtf8, RectRel dstRel,
        float alignX, float alignY, float offsetX, float offsetY, TintSet tints,
        float z = 0.0f, Render::Layer layer = Render::Layer::HUD);

    HitKey Key() const { return key_; }
    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }
    bool Enabled() const { return enabled_; }
    void SetEnabled(bool e) { enabled_ = e; }
    Render::RectF Rect() const { return rect_; }
    Render::RectF ContentRect() const { return content_; }

private:
    void ApplyVisualState(UIFrame& frame);

    struct SpriteSlot { int idx = -1; TintSet tints{}; };
    struct TextSlot   { int idx = -1; TintSet tints{}; };

private:
    HitKey key_ = 0;
    bool enabled_ = true;
    bool visible_ = false;
    bool hovered_ = false;

    Render::RectF rect_{0,0,0,0};
    Render::RectF content_{0,0,0,0};

    int hitIdx_ = -1;

    std::vector<SpriteSlot> sprites_;
    std::vector<TextSlot> texts_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_UIBUTTONWIDGET_H
