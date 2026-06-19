#include "PlayScene.h"

#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "SceneManager.h"

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    elapsedTime += deltaTime;

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
    renderer.DrawCenteredText(L"PLAY SCENE", 80, 50, 34, RGB(255, 255, 255));
    renderer.DrawTextLine(L"Placeholder world is running.", 40, 170, 22, RGB(20, 35, 45));
    renderer.DrawTextLine(L"Press G for GameOver, R to reset, Esc to quit.", 40, 210, 20, RGB(20, 35, 45));
    renderer.DrawCenteredText(L"[ Mario / Renderer / World systems go here ]", 320, 40, 20, RGB(255, 245, 210));
    renderer.End();
}
