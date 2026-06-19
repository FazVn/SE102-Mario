#pragma once

#include "IScene.h"

class WinScene : public IScene
{
public:
    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;
};
