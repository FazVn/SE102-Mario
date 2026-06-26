#pragma once

#include "IScene.h"
#include "../Core/FontManager.h"
#include "../Core/SpriteManager.h"
#include "../Core/TextureManager.h"
#include "../Objects/Environment/Platform.h"
#include "../Objects/Player/Mario.h"

#include <string>
#include <vector>
#include <windows.h>

class PlayScene : public IScene
{
public:
    explicit PlayScene(TextureManager& textures);

    void Load() override;
    void Unload() override;

    void Update(SceneManager& sceneManager,
        const Input& input,
        float deltaTime) override;

    void Render(Renderer& renderer,
        HWND windowHandle) override;

private:
    void BuildTestLevel();

    TextureManager& textureManager;
    SpriteManager spriteManager;
    FontManager fontManager;
    std::wstring marioFontFamily;

    Mario mario;
    std::vector<Platform> platforms;
    std::vector<RectF> solidBounds;

    bool assetsLoaded = false;
};