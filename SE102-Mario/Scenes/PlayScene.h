#pragma once

#include "../Core/FontManager.h"
#include "../Core/SpriteManager.h"
#include "../Core/TextureManager.h"
#include "../Objects/Player/Mario.h"
#include "IScene.h"

class PlayScene : public IScene
{
public:
    void Load() override;
    void Unload() override;
    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    bool assetsLoaded = false;
    std::wstring marioFontFamily = L"Consolas";

    TextureManager textureManager;
    SpriteManager spriteManager;
    FontManager fontManager;
    Mario mario;
};
