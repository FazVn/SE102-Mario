#include "SceneFactory.h"

#include "../Core/TextureManager.h"

#include "GameOverScene.h"
#include "MenuScene.h"
#include "PauseScene.h"
#include "PlayScene.h"
#include "WinScene.h"

SceneFactory::SceneFactory(TextureManager& textures)
    : textureManager(textures)
{
}

std::unique_ptr<IScene> SceneFactory::Create(SceneId sceneId, int score) const
{
    switch (sceneId)
    {
    case SceneId::Menu:
        return std::make_unique<MenuScene>(textureManager);

    case SceneId::Play:
        return std::make_unique<PlayScene>(textureManager);

    case SceneId::Pause:                        
        return std::make_unique<PauseScene>();

    case SceneId::GameOver:
        return std::make_unique<GameOverScene>();

    case SceneId::Win:
        return std::make_unique<WinScene>(score);

    default:
        return std::make_unique<MenuScene>(textureManager);
    }
}
