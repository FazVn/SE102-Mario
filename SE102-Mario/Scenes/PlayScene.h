#pragma once

#include "../Core/Camera.h"
#include "../Core/FontManager.h"
#include "../Core/SpriteManager.h"
#include "../Core/TextureManager.h"
#include "../Objects/GameObject.h"
#include "../Objects/Player/Mario.h"
#include "IScene.h"

#include <string>
#include <vector>

class Sprite;

class PlayScene : public IScene
{
public:
    void Load() override;
    void Unload() override;
    void Update(SceneManager& sceneManager, const Input& input, float deltaTime) override;
    void Render(Renderer& renderer, HWND windowHandle) override;

private:
    struct SpriteInstance
    {
        const Sprite* sprite = nullptr;
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    struct TiledSpriteInstance
    {
        const Sprite* sprite = nullptr;
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        int tileWidth = 32;
        int tileHeight = 32;
    };

    struct CoinInstance
    {
        SpriteInstance visual;
        bool active = true;
    };

    struct EnemyInstance
    {
        const Sprite* walkFrame1 = nullptr;
        const Sprite* walkFrame2 = nullptr;
        float x = 0.0f;
        float y = 0.0f;
        float width = 32.0f;
        float height = 32.0f;
        float velocityX = -42.0f;
        float velocityY = 0.0f;
        float animationTime = 0.0f;
        bool active = true;
    };

    bool LoadLevelFromFile(const std::wstring& resourceRelativePath);
    void BuildFallbackLevel();
    void ClearLevel();
    void AddSolidRect(float x, float y, float width, float height);
    void AddSprite(const std::string& spriteId, float x, float y, float width, float height);
    void AddSolidSprite(const std::string& spriteId, float x, float y, float width, float height);
    void AddTiledSprite(const std::string& spriteId, float x, float y, float width, float height, int tileWidth, int tileHeight, bool solid);
    void AddPipe(float x, float y, float width, float height);
    void AddStaircase(float x, float y, int steps, int direction);
    void AddEnemy(const std::string& enemyType, float x, float y);
    void AddCoin(float x, float y);
    void UpdateCamera();
    bool UpdateEnemies(float deltaTime);
    void UpdateCoins();
    void UpdateMarioSprite(float deltaTime);
    void RenderSpriteInstance(Renderer& renderer, const SpriteInstance& instance);
    void RenderTiledSpriteInstance(Renderer& renderer, const TiledSpriteInstance& instance);
    bool IsVisibleInCamera(float x, float width) const;

    bool assetsLoaded = false;
    std::wstring marioFontFamily = L"Consolas";

    TextureManager textureManager;
    SpriteManager spriteManager;
    FontManager fontManager;
    Camera camera;
    Mario mario;

    std::vector<SpriteInstance> backgroundSprites;
    std::vector<SpriteInstance> solidSprites;
    std::vector<TiledSpriteInstance> tiledSprites;
    std::vector<CoinInstance> coins;
    std::vector<EnemyInstance> enemies;
    std::vector<RectF> solidBounds;

    RectF winBounds{};
    float levelWidth = 800.0f;
    float levelHeight = 480.0f;
    float spawnX = 96.0f;
    float spawnY = 368.0f;
    float marioAnimationTime = 0.0f;
    int coinsCollected = 0;
};
