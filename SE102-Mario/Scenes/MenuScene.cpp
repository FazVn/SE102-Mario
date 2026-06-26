#include "MenuScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "../Core/TextureManager.h"

#include "SceneManager.h"

MenuScene::MenuScene(TextureManager& textures)
{
}

void MenuScene::Load()
{
}

void MenuScene::Unload()
{
}

void MenuScene::Update(SceneManager& sceneManager,
    const Input& input,
    float)
{
    if (input.WasKeyPressed(VK_UP))
    {
        selected--;
        if (selected < 0)
            selected = 1;
    }

    if (input.WasKeyPressed(VK_DOWN))
    {
        selected++;
        if (selected > 1)
            selected = 0;
    }

    if (input.WasKeyPressed(VK_RETURN))
    {
        if (selected == 0)
        {
            sceneManager.RequestChange(SceneId::Play);
        }
        else
        {
            PostQuitMessage(0);
        }
    }
}

void MenuScene::Render(Renderer& renderer,
    HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(36, 56, 88));

    // TITLE
    renderer.DrawCenteredText(
        L"MARIO GAME",
        60,
        60,
        40,
        RGB(255, 230, 80));

    //-----------------------------------
    // PLAY
    //-----------------------------------
    if (selected == 0)
    {
        renderer.DrawCenteredText(
            L"> PLAY <",
            180,
            40,
            30,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"PLAY",
            180,
            40,
            30,
            RGB(180, 180, 180));
    }

    //-----------------------------------
    // EXIT
    //-----------------------------------
    if (selected == 1)
    {
        renderer.DrawCenteredText(
            L"> EXIT <",
            240,
            40,
            30,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"EXIT",
            240,
            40,
            30,
            RGB(180, 180, 180));
    }

    // HƯỚNG DẪN
    renderer.DrawCenteredText(
        L"UP / DOWN : Select",
        360,
        30,
        18,
        RGB(220, 220, 220));

    renderer.DrawCenteredText(
        L"ENTER : Confirm",
        390,
        30,
        18,
        RGB(220, 220, 220));

    renderer.End();
}