#include "WinScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"
#include <iomanip>
#include <sstream>
#include <windows.h> 

namespace
{
    std::wstring FormatScore(int score)
    {
        std::wostringstream stream;
        stream << std::setfill(L'0') << std::setw(6) << score;
        return stream.str();
    }
}

WinScene::WinScene(int score)
    : score(score)
{
}

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

    renderer.DrawCenteredText(
        L"YOU WIN!",
        60,
        60,
        42,
        RGB(100, 255, 150));

    renderer.DrawCenteredText(
        L"SCORE  " + FormatScore(score),
        125,
        40,
        28,
        RGB(255, 255, 255));


    if (selected == 0)
    {
        renderer.DrawCenteredText(
            L"> RESTART <",
            185,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"RESTART",
            185,
            40,
            28,
            RGB(180, 180, 180));
    }


    if (selected == 1)
    {
        renderer.DrawCenteredText(
            L"> RETURN TO MENU <",
            235,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"RETURN TO MENU",
            235,
            40,
            28,
            RGB(180, 180, 180));
    }


    if (selected == 2)
    {
        renderer.DrawCenteredText(
            L"> EXIT <",
            285,
            40,
            28,
            RGB(255, 230, 80));
    }
    else
    {
        renderer.DrawCenteredText(
            L"EXIT",
            285,
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
        L"ENTER : Confirm",
        390,
        30,
        18,
        RGB(220, 220, 220));

    renderer.End();
}
