#pragma once

#include "IScene.h"
#include "SceneFactory.h"
#include "SceneId.h"
#include <memory>
#include <optional>

class Input;
class Renderer;
struct HWND__;
using HWND = HWND__*;

class SceneManager
{
public:
    void SetFactory(std::unique_ptr<SceneFactory> newFactory);
    void RequestChange(SceneId sceneId);
    void ApplyPendingChange();

    void Update(const Input& input, float deltaTime);
    void Render(Renderer& renderer, HWND windowHandle);

private:
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<IScene> currentScene;
    std::optional<SceneId> pendingScene;
};
