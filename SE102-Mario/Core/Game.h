#pragma once

#include "Input.h"
#include "Renderer.h"
#include "../Scenes/SceneManager.h"
#include <windows.h>

class Game
{
public:
    explicit Game(HINSTANCE instance);

    bool Initialize(int showCommand);
    int Run();

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    void Update(float deltaTime);
    void Render();

    HINSTANCE instance = nullptr;
    HWND windowHandle = nullptr;
    bool isRunning = false;

    Input input;
    Renderer renderer;
    SceneManager sceneManager;
};
