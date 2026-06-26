#include "WinScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"
#include <windows.h> 

void WinScene::Update(SceneManager& sceneManager, const Input& input, float)
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
}

void WinScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(36, 56, 88));

    // TITLE
    renderer.DrawCenteredText(
        L"YOU WIN!",
        60,
        60,
        42,
        RGB(100, 255, 150));

    //-----------------------------------
    // NEXT LEVEL / RESTART
    //-----------------------------------
    if (selected == 0)
    {
        renderer.DrawCenteredText(
            L"> RESTART <",
            170,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"RESTART",
            170,
            40,
            28,
            RGB(180, 180, 180));
    }

    //-----------------------------------
    // RETURN TO MENU
    //-----------------------------------
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

    //-----------------------------------
    // EXIT
    //-----------------------------------
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