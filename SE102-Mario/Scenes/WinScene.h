#pragma once

#include "IScene.h"

class WinScene : public IScene
{
public:
    explicit WinScene(int score);

    void Load() override {}
    void Unload() override {}

    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    int score = 0;
    int selected = 0;
};
