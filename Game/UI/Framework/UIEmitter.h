// Game/UI/Framework/UIEmitter.h
#ifndef GAME_UI_FRAMEWORK_UIEMITTER_H
#define GAME_UI_FRAMEWORK_UIEMITTER_H

#include "UIFrame.h"
#include "Render/Draw/DrawList.h"

namespace Salt2D::Game::UI {

class UIEmitter {
public:
    void Emit(Render::DrawList& drawList,
        ID3D11ShaderResourceView* whiteSRV,
        const UIFrame& frame);
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIEMITTER_H
