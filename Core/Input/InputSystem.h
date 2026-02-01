// Core/Input/InputSystem.h
#ifndef CORE_INPUT_INPUTSYSTEM_H
#define CORE_INPUT_INPUTSYSTEM_H

#include <cstdint>
#include <algorithm>
#include "InputState.h"

namespace Salt2D::Core {

class InputSystem {
public:
    InputState state;

    void BeginFrame() { state.BeginFrame(); }

    void OnMouseMove(int x, int y) {
        state.mouseDX += (x - state.mouseX);
        state.mouseDY += (y - state.mouseY);
        state.mouseX = x;
        state.mouseY = y;
    }

    void OnMouseWheel(int delta) { state.wheel += delta; }

    void OnKeyDown(uint16_t vk) { state.curr[vk] = 1; }
    void OnKeyUp  (uint16_t vk) { state.curr[vk] = 0; }

    void ClearAll() {
        state.curr.fill(0);
    }
};

} // namespace Salt2D::Core

#endif // CORE_INPUT_INPUTSYSTEM_H
