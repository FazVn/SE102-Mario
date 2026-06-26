#include "Game.h"
#include "GameConfig.h"
#include "../Scenes/SceneFactory.h"

#include <chrono>

Game::Game(HINSTANCE appInstance)
    : instance(appInstance)
{
}

bool Game::Initialize(int showCommand)
{
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.lpfnWndProc = Game::WindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszClassName = L"MarioGameWindowClass";

    if (!RegisterClassExW(&windowClass))
    {
        return false;
    }

    RECT windowRect{ 0, 0, GameConfig::WindowWidth, GameConfig::WindowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    windowHandle = CreateWindowExW(
        0,
        windowClass.lpszClassName,
        GameConfig::WindowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        instance,
        this);

    if (!windowHandle)
    {
        return false;
    }

    ShowWindow(windowHandle, showCommand);
    UpdateWindow(windowHandle);


    if (!textureManager.LoadFromDefinitionFile(
        L"Resources\\definitions\\textures.txt"))
    {
        MessageBoxW(
            windowHandle,
            L"Cannot load textures.txt",
            L"Error",
            MB_OK | MB_ICONERROR);

        return false;
    }


    sceneManager.SetFactory(
        std::make_unique<SceneFactory>(textureManager));

    sceneManager.RequestChange(SceneId::Menu);
    sceneManager.ApplyPendingChange();

    return true;
}

int Game::Run()
{
    isRunning = true;

    MSG message{};

    auto previousTime = std::chrono::high_resolution_clock::now();

    while (isRunning)
    {
        input.BeginFrame();

        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                isRunning = false;
                break;
            }

            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        auto currentTime = std::chrono::high_resolution_clock::now();

        std::chrono::duration<float> elapsed =
            currentTime - previousTime;

        previousTime = currentTime;

        Update(elapsed.count());

        Render();

        Sleep(16);
    }

    return static_cast<int>(message.wParam);
}

LRESULT CALLBACK Game::WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    Game* game = nullptr;

    if (message == WM_NCCREATE)
    {
        auto* createStruct =
            reinterpret_cast<CREATESTRUCTW*>(lParam);

        game =
            static_cast<Game*>(createStruct->lpCreateParams);

        SetWindowLongPtrW(
            hwnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(game));
    }
    else
    {
        game =
            reinterpret_cast<Game*>(
                GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (game)
    {
        return game->HandleMessage(
            hwnd,
            message,
            wParam,
            lParam);
    }

    return DefWindowProcW(
        hwnd,
        message,
        wParam,
        lParam);
}

LRESULT Game::HandleMessage(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:

        if ((lParam & (1 << 30)) == 0)
        {
            input.SetKeyState(wParam, true);
        }

        return 0;

    case WM_KEYUP:

        input.SetKeyState(wParam, false);

        return 0;

    case WM_DESTROY:

        isRunning = false;

        PostQuitMessage(0);

        return 0;

    default:

        return DefWindowProcW(
            hwnd,
            message,
            wParam,
            lParam);
    }
}

void Game::Update(float deltaTime)
{

    sceneManager.Update(input, deltaTime);

    sceneManager.ApplyPendingChange();
}

void Game::Render()
{
    sceneManager.Render(
        renderer,
        windowHandle);
}