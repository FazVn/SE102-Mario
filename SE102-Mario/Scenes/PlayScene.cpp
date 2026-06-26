#include "PlayScene.h"

#include "../Core/GameConfig.h"
#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "../Core/Sprite.h"
#include "SceneManager.h"

#include <string>

namespace
{
    constexpr int GroundTileSize = 32;
    constexpr float GroundHeight = 64.0f;
    constexpr float RaisedPlatformHeight = 32.0f;
    constexpr float MaxPhysicsDeltaTime = 0.05f;
}

PlayScene::PlayScene(TextureManager& textures)
    : textureManager(textures)
{
}

void PlayScene::Load()
{
    assetsLoaded = spriteManager.LoadFromDefinitionFile(L"definitions/sprites.txt", textureManager);

    if (fontManager.Load("mario", L"fonts/SS-Mario-Regular.otf", L"SS Mario"))
    {
        const std::wstring* loadedFamily = fontManager.GetFamily("mario");
        if (loadedFamily)
        {
            marioFontFamily = *loadedFamily;
        }
    }

    BuildTestLevel();
}

void PlayScene::Unload()
{
    mario.SetSprite(nullptr);
    platforms.clear();
    solidBounds.clear();
    spriteManager.Clear();
    fontManager.Clear();
}

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    if (input.WasKeyPressed('G'))
    {
        sceneManager.RequestChange(SceneId::GameOver);
    }

    if (input.WasKeyPressed('R'))
    {
        sceneManager.RequestChange(SceneId::Play);
    }

    if (input.WasKeyPressed('W'))
        sceneManager.RequestChange(SceneId::Win);

    const float physicsDeltaTime = deltaTime > MaxPhysicsDeltaTime ? MaxPhysicsDeltaTime : deltaTime;
    mario.Update(input, physicsDeltaTime);
    mario.ResolveSolidCollisions(solidBounds);
}

void PlayScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(92, 148, 252));

    if (const Sprite* cloud = spriteManager.Get("scenery.cloud"))
    {
        renderer.DrawSprite(*cloud, 68, 70, 120, 61);
        renderer.DrawSprite(*cloud, 560, 86, 96, 49);
    }

    for (Platform& platform : platforms)
    {
        platform.Render(renderer);
    }

    mario.Render(renderer);

    renderer.End();
}

void PlayScene::BuildTestLevel()
{
    platforms.clear();
    solidBounds.clear();

    const Sprite* groundSprite = spriteManager.Get("block.dirt");
    const float groundTop = static_cast<float>(GameConfig::WindowHeight) - GroundHeight;

    platforms.emplace_back(0.0f, groundTop, static_cast<float>(GameConfig::WindowWidth), GroundHeight,
        groundSprite, GroundTileSize, GroundTileSize);
    platforms.emplace_back(320.0f, groundTop - RaisedPlatformHeight, 160.0f, RaisedPlatformHeight,
        groundSprite, GroundTileSize, GroundTileSize);

    for (const Platform& platform : platforms)
    {
        solidBounds.push_back(platform.GetBoundingBox());
    }

    const Sprite* marioSprite = spriteManager.Get("mario.super.stand");
    mario.SetSprite(marioSprite);

    if (marioSprite)
    {
        mario.SetRenderSize(marioSprite->GetWidth(), marioSprite->GetHeight());
    }
    else
    {
        mario.SetRenderSize(Mario::DefaultRenderWidth, Mario::DefaultRenderHeight);
    }

    mario.SetPosition(96.0f, groundTop - mario.GetHeight());
    mario.SetOnGround(true);
}
