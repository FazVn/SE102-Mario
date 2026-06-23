#include "PlayScene.h"

#include "../Core/Input.h"
#include "../Core/RenderOptions.h"
#include "../Core/Renderer.h"
#include "../Core/Sprite.h"
#include "SceneManager.h"

#include <string>

void PlayScene::Load()
{
    assetsLoaded = textureManager.LoadFromDefinitionFile(L"definitions/textures.txt");
    assetsLoaded = spriteManager.LoadFromDefinitionFile(L"definitions/sprites.txt", textureManager) && assetsLoaded;

    if (fontManager.Load("mario", L"fonts/SS-Mario-Regular.otf", L"SS Mario"))
    {
        const std::wstring* loadedFamily = fontManager.GetFamily("mario");
        if (loadedFamily)
        {
            marioFontFamily = *loadedFamily;
        }
    }

    mario.SetPosition(100.0f, 100.0f);
    mario.SetRenderSize(57, 90);
    mario.SetSprite(spriteManager.Get("mario.super.stand"));
}

void PlayScene::Unload()
{
    mario.SetSprite(nullptr);
    spriteManager.Clear();
    textureManager.Clear();
    fontManager.Clear();
}

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    (void)deltaTime;

    if (input.WasKeyPressed('G'))
    {
        sceneManager.RequestChange(SceneId::GameOver);
    }

    if (input.WasKeyPressed('R'))
    {
        sceneManager.RequestChange(SceneId::Play);
    }

}

void PlayScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(92, 148, 252));

    if (const Sprite* sky = spriteManager.Get("background.sky"))
    {
        RenderOptions skyOptions;
        skyOptions.scale = 3.12f;
        renderer.DrawSprite(*sky, 0, 0, skyOptions);
    }

    if (const Sprite* mountain = spriteManager.Get("background.mountain"))
    {
        RenderOptions mountainOptions;
        mountainOptions.scale = 1.45f;
        renderer.DrawSprite(*mountain, 390, 175, mountainOptions);
    }

    if (const Sprite* cloud = spriteManager.Get("scenery.cloud"))
    {
        renderer.DrawSprite(*cloud, 70, 96);
    }

    if (const Sprite* tile = spriteManager.Get("tile.question"))
    {
        RenderOptions tileOptions;
        tileOptions.scale = 2.0f;
        for (int x = 0; x < 800; x += 32)
        {
            renderer.DrawSprite(*tile, x, 416, tileOptions);
        }
    }

    mario.Render(renderer);

    if (const Sprite* brick = spriteManager.Get("block.gold.blackborder.brick"))
    {
        renderer.DrawSprite(*brick, 200, 100, 48, 48);
    }

    renderer.DrawCenteredText(L"Sprite render test", 26, 40, 28, RGB(255, 255, 255), marioFontFamily.c_str(), FW_NORMAL);
    renderer.DrawTextLine(assetsLoaded ? L"mario.super.stand at (100, 100)" : L"Some assets failed to load", 36, 300, 18,
        assetsLoaded ? RGB(30, 55, 60) : RGB(160, 20, 20), marioFontFamily.c_str(), FW_NORMAL);
    renderer.DrawTextLine(L"block.gold.blackborder.brick at (200, 100)", 36, 330, 18, RGB(30, 55, 60));
    renderer.DrawTextLine(L"Press G for GameOver, R to reset, Esc to quit.", 36, 360, 18, RGB(30, 55, 60));

    renderer.End();
}
