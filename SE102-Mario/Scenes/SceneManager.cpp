#include "SceneManager.h"

#include "IScene.h"
#include "SceneFactory.h"

void SceneManager::SetFactory(std::unique_ptr<SceneFactory> newFactory)
{
    factory = std::move(newFactory);
}

void SceneManager::RequestChange(SceneId sceneId)
{
    pendingScene = sceneId;
}

void SceneManager::ApplyPendingChange()
{
    if (!pendingScene || !factory)
    {
        return;
    }

    if (currentScene)
    {
        currentScene->Unload();
    }

    currentScene = factory->Create(*pendingScene);
    currentScene->Load();
    pendingScene.reset();
}

void SceneManager::Update(const Input& input, float deltaTime)
{
    if (currentScene)
    {
        currentScene->Update(*this, input, deltaTime);
    }
}

void SceneManager::Render(Renderer& renderer, HWND windowHandle)
{
    if (currentScene)
    {
        currentScene->Render(renderer, windowHandle);
    }
}

void SceneManager::SetTextureManager(TextureManager* manager)
{
    textureManager = manager;
}

TextureManager& SceneManager::GetTextureManager()
{
    return *textureManager;
}