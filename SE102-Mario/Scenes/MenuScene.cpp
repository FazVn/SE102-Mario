#include "MenuScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"

void MenuScene::Update(SceneManager& sceneManager, const Input& input, float)
{
    if (input.WasKeyPressed(VK_RETURN))
    {
        sceneManager.RequestChange(SceneId::Play);
    }
}

void MenuScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(36, 56, 88));
    renderer.DrawCenteredText(L"MARIO GAME", 110, 70, 44, RGB(255, 235, 120));
    renderer.DrawCenteredText(L"Press Enter to Play", 220, 40, 24, RGB(245, 245, 245));
    renderer.DrawCenteredText(L"Esc to Quit", 270, 32, 18, RGB(190, 205, 220));
    renderer.End();
}
