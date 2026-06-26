#pragma once

#include "IScene.h"
#include <windows.h>

class TextureManager;

class MenuScene : public IScene
{
public:
    explicit MenuScene(TextureManager& textures);

    void Load() override;
    void Unload() override;

    void Update(SceneManager& sceneManager,
        const Input& input,
        float deltaTime) override;

    void Render(Renderer& renderer,
        HWND windowHandle) override;

private:
    int selected = 0;
};