// Core/Input/InputState.h
#ifndef CORE_INPUT_INPUTSTATE_H
#define CORE_INPUT_INPUTSTATE_H

#include <array>
#include <cstdint>

namespace Salt2D::Core {

enum class Key : uint16_t {
    W='W', A='A', S='S', D='D',
    Q='Q', E='E', F='F', R='R', M='M',
    C='C', V='V', L='L',
    Shift=VK_SHIFT,
    Ctrl=VK_CONTROL,
    Alt=VK_MENU,
    Space=VK_SPACE,
    Escape=VK_ESCAPE,
    Enter=VK_RETURN,

    Up=VK_UP, Down=VK_DOWN, Left=VK_LEFT, Right=VK_RIGHT,

    F1=VK_F1, F2=VK_F2, F3=VK_F3,

    LButton=VK_LBUTTON,
    RButton=VK_RBUTTON,
};

static uint16_t KeyToVK(Key k) { return static_cast<uint16_t>(k); }

struct InputState {
    std::array<uint8_t, 256> curr{};
    std::array<uint8_t, 256> prev{};

    int mouseX = 0;
    int mouseY = 0;
    int mouseDX = 0;
    int mouseDY = 0;
    int wheel   = 0;

    void BeginFrame() {
        prev = curr;
        mouseDX = 0;
        mouseDY = 0;
        wheel = 0;
    }

    bool Down    (uint16_t vk) const { return curr[vk] != 0; }
    bool Pressed (uint16_t vk) const { return curr[vk] != 0 && prev[vk] == 0; }
    bool Released(uint16_t vk) const { return curr[vk] == 0 && prev[vk] != 0; }

    bool Down    (Key k) const { return Down(KeyToVK(k)); }
    bool Pressed (Key k) const { return Pressed(KeyToVK(k)); }
    bool Released(Key k) const { return Released(KeyToVK(k)); }
};

} // namespace Salt2D::Core

#endif // CORE_INPUT_INPUTSTATE_H
