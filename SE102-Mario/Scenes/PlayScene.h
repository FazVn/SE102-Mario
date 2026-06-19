#pragma once

#include "IScene.h"

class PlayScene : public IScene
{
public:
    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    float elapsedTime = 0.0f;
};
