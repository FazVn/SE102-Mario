#include "WinScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"

void WinScene::Update(SceneManager&, const Input&, float)
{
}

void WinScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(38, 98, 72));
    renderer.DrawCenteredText(L"YOU WIN", 140, 60, 42, RGB(255, 245, 180));
    renderer.DrawCenteredText(L"Esc to Quit", 230, 40, 24, RGB(245, 245, 245));
    renderer.End();
}
