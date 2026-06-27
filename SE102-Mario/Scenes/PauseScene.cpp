#include "PauseScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"
#include <windows.h>

void PauseScene::Update(SceneManager& sceneManager, const Input& input, float)
{
    if (input.WasKeyPressed(VK_UP))
    {
        selected--;
        if (selected < 0)
            selected = 2;
    }

    if (input.WasKeyPressed(VK_DOWN))
    {
        selected++;
        if (selected > 2)
            selected = 0;
    }

    if (input.WasKeyPressed(VK_RETURN))
    {
        if (selected == 0)
        {
            sceneManager.RequestChange(SceneId::Play);
        }
        else if (selected == 1)
        {
            sceneManager.RequestChange(SceneId::Menu);
        }
        else
        {
           
            PostQuitMessage(0);
        }
    }

    if (input.WasKeyPressed(VK_ESCAPE))
    {
        sceneManager.RequestChange(SceneId::Play);
    }
}

void PauseScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(20, 20, 40));

    renderer.DrawCenteredText(
        L"PAUSED",
        60,
        60,
        42,
        RGB(255, 255, 255));


    if (selected == 0)
    {
        renderer.DrawCenteredText(
            L"> RESUME <",
            170,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"RESUME",
            170,
            40,
            28,
            RGB(180, 180, 180));
    }


    if (selected == 1)
    {
        renderer.DrawCenteredText(
            L"> RETURN TO MENU <",
            220,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"RETURN TO MENU",
            220,
            40,
            28,
            RGB(180, 180, 180));
    }


    if (selected == 2)
    {
        renderer.DrawCenteredText(
            L"> EXIT <",
            270,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"EXIT",
            270,
            40,
            28,
            RGB(180, 180, 180));
    }


    renderer.DrawCenteredText(
        L"UP / DOWN : Select",
        360,
        30,
        18,
        RGB(220, 220, 220));

    renderer.DrawCenteredText(
        L"ENTER : Confirm      ESC : Resume",
        390,
        30,
        18,
        RGB(220, 220, 220));

    renderer.End();
}