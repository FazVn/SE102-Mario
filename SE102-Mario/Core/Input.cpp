#include "Input.h"

void Input::BeginFrame()
{
    previousKeys = currentKeys;
}

void Input::SetKeyState(WPARAM key, bool isDown)
{
    if (key < currentKeys.size())
    {
        currentKeys[static_cast<size_t>(key)] = isDown;
    }
}

bool Input::IsKeyDown(int key) const
{
    return key >= 0 && key < static_cast<int>(currentKeys.size()) && currentKeys[static_cast<size_t>(key)];
}

bool Input::WasKeyPressed(int key) const
{
    if (key < 0 || key >= static_cast<int>(currentKeys.size()))
    {
        return false;
    }

    const auto index = static_cast<size_t>(key);
    return currentKeys[index] && !previousKeys[index];
}
