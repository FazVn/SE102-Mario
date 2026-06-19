#pragma once

#include "SceneId.h"
#include <memory>

class IScene;

class SceneFactory
{
public:
    std::unique_ptr<IScene> Create(SceneId sceneId) const;
};
