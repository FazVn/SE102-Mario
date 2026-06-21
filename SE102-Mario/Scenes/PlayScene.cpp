#include "PlayScene.h"

#include "../Core/Input.h"
#include "../Core/RenderOptions.h"
#include "../Core/Renderer.h"
#include "../Core/Sprite.h"
#include "SceneManager.h"

#include <string>

void PlayScene::Load()
{
    elapsedTime = 0.0f;
    assetsLoaded = textureManager.LoadFromDefinitionFile(L"definitions/textures.txt");
    assetsLoaded = spriteManager.LoadFromDefinitionFile(L"definitions/sprites.txt", textureManager) && assetsLoaded;

    if (fontManager.Load("mario", L"fonts/SuperMario256.ttf", L"Super Mario 256"))
    {
        const std::wstring* loadedFamily = fontManager.GetFamily("mario");
        if (loadedFamily)
        {
            marioFontFamily = *loadedFamily;
        }
    }

    marioIdleAnimation.Clear();
    marioIdleAnimation.AddFrame(spriteManager.Get("mario.small.stand"), 0.25f);
}

void PlayScene::Unload()
{
    marioIdleAnimation.Clear();
    spriteManager.Clear();
    textureManager.Clear();
    fontManager.Clear();
}

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    elapsedTime += deltaTime;
    marioIdleAnimation.Update(deltaTime);

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

    if (const Sprite* mario = marioIdleAnimation.GetCurrentSprite())
    {
        RenderOptions marioOptions;
        marioOptions.scale = 3.0f;
        renderer.DrawSprite(*mario, 160, 368, marioOptions);
    }

    renderer.DrawCenteredText(L"MARIO ASSET TEST", 26, 40, 28, RGB(255, 255, 255), marioFontFamily.c_str(), FW_NORMAL);
    renderer.DrawTextLine(assetsLoaded ? L"PNG sprites + TTF font loaded" : L"Some assets failed to load", 36, 74, 18,
        assetsLoaded ? RGB(30, 55, 60) : RGB(160, 20, 20), marioFontFamily.c_str(), FW_NORMAL);
    renderer.DrawTextLine(L"Wide text path ready: Xin chao Mario!", 36, 104, 18, RGB(30, 55, 60));
    renderer.DrawTextLine(L"Press G for GameOver, R to reset, Esc to quit.", 36, 136, 18, RGB(30, 55, 60));

    renderer.End();
}
