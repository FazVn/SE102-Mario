#pragma once

#include "SceneId.h"

#include <memory>

class IScene;
class TextureManager;

class SceneFactory
{
public:
    explicit SceneFactory(TextureManager& textures);

    std::unique_ptr<IScene> Create(SceneId sceneId, int score = 0) const;

private:
    TextureManager& textureManager;
};
