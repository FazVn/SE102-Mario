#pragma once

#include "../Core/Animation.h"
#include "../Core/FontManager.h"
#include "../Core/SpriteManager.h"
#include "../Core/TextureManager.h"
#include "IScene.h"

class PlayScene : public IScene
{
public:
    void Load() override;
    void Unload() override;
    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    float elapsedTime = 0.0f;
    bool assetsLoaded = false;
    std::wstring marioFontFamily = L"Consolas";

    TextureManager textureManager;
    SpriteManager spriteManager;
    FontManager fontManager;
    Animation marioIdleAnimation;
};
