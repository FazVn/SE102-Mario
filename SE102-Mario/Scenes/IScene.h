#pragma once

class Input;
class Renderer;
class SceneManager;
struct HWND__;
using HWND = HWND__*;

class IScene
{
public:
    virtual ~IScene() = default;

    virtual void Load() {}
    virtual void Unload() {}
    virtual void Update(SceneManager& sceneManager, const Input& input, float deltaTime) = 0;
    virtual void Render(Renderer& renderer, HWND windowHandle) = 0;
};
