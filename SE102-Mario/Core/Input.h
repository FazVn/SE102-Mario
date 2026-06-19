#pragma once

#include <array>
#include <windows.h>

class Input
{
public:
    void BeginFrame();
    void SetKeyState(WPARAM key, bool isDown);
    bool IsKeyDown(int key) const;
    bool WasKeyPressed(int key) const;

private:
    std::array<bool, 256> currentKeys{};
    std::array<bool, 256> previousKeys{};
};
