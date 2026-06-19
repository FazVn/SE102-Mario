#include "GameOverScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"

void GameOverScene::Update(SceneManager& sceneManager, const Input& input, float)
{
    if (input.WasKeyPressed(VK_RETURN))
    {
        sceneManager.RequestChange(SceneId::Play);
    }
}

void GameOverScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(45, 45, 48));
    renderer.DrawCenteredText(L"GAME OVER", 140, 60, 42, RGB(255, 100, 100));
    renderer.DrawCenteredText(L"Press Enter to Retry", 230, 40, 24, RGB(245, 245, 245));
    renderer.End();
}
