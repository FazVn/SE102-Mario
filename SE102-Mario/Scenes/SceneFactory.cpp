#include "SceneFactory.h"

#include "GameOverScene.h"
#include "MenuScene.h"
#include "PlayScene.h"
#include "WinScene.h"

std::unique_ptr<IScene> SceneFactory::Create(SceneId sceneId) const
{
    switch (sceneId)
    {
    case SceneId::Menu:
        return std::make_unique<MenuScene>();
    case SceneId::Play:
        return std::make_unique<PlayScene>();
    case SceneId::GameOver:
        return std::make_unique<GameOverScene>();
    case SceneId::Win:
        return std::make_unique<WinScene>();
    default:
        return std::make_unique<MenuScene>();
    }
}
