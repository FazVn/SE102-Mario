#pragma once

#include "IScene.h"

class PauseScene : public IScene
{
public:
    void Load() override {}
    void Unload() override {}

    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    int selected = 0;
};