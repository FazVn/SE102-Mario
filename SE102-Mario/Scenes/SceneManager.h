#pragma once

#include "IScene.h"
#include "SceneFactory.h"
#include "../Core/TextureManager.h"
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
    void RequestChange(SceneId sceneId, int score = 0);
    void ApplyPendingChange();

    void Update(const Input& input, float deltaTime);
    void Render(Renderer& renderer, HWND windowHandle);

    void SetTextureManager(TextureManager* manager);
    TextureManager& GetTextureManager();

private:
    std::unique_ptr<SceneFactory> factory;
    std::unique_ptr<IScene> currentScene;
    std::optional<SceneId> pendingScene;
    int pendingScore = 0;

    TextureManager* textureManager = nullptr;
};
