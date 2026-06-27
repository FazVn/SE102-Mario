#pragma once

#include "IScene.h"
#include "../Core/Camera.h"
#include "../Core/FontManager.h"
#include "../Core/SpriteManager.h"
#include "../Core/TextureManager.h"
#include "../Objects/Blocks/QuestionBlock.h"
#include "../Objects/Enemies/Enemy.h"
#include "../Objects/GameObject.h"
#include "../Objects/Items/Coin.h"
#include "../Objects/Items/Mushroom.h"
#include "../Objects/Items/Star.h"
#include "../Objects/Player/Mario.h"

#include <memory>
#include <string>
#include <vector>
#include <windows.h>

class Sprite;

class PlayScene : public IScene
{
public:
    explicit PlayScene(TextureManager& textures);

    void Load() override;
    void Unload() override;

    void Update(SceneManager& sceneManager,
        const Input& input,
        float deltaTime) override;

    void Render(Renderer& renderer,
        HWND windowHandle) override;

    void AddScore(int amount);
    void AddCoin();
    void OnEnemyDefeated(Enemy& enemy);
    void OnEnemyStomped(Enemy& enemy);
    void OnShellKicked(Enemy& enemy, float direction, const RectF& marioBounds);
    void AwardTimeBonus();

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

    struct FireballInstance
    {
        float x = 0.0f;
        float y = 0.0f;
        float velocityX = 0.0f;
        float animationTime = 0.0f;
        float lifeTime = 0.0f;
        bool active = true;
    };

    struct PracticeCheckpoint
    {
        bool active = false;
        float x = 0.0f;
        float y = 0.0f;
        Mario::Form form = Mario::Form::Small;
        bool onGround = false;
        int score = 0;
        int coinCount = 0;
        int timeLeft = 400;
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
    void AddQuestionBlock(float x, float y, QuestionBlock::Content content);
    void SpawnQuestionBlockContent(const QuestionBlock& block);
    void UpdateCamera();
    void UpdateQuestionBlocks(float deltaTime);
    bool UpdateEnemies(float deltaTime);
    void UpdateItems(float deltaTime);
    void SpawnFireball();
    void UpdateFireballs(float deltaTime);
    void RenderFireballs(Renderer& renderer);
    void UpdateDebugMenu(const Input& input);
    void RenderDebugMenu(Renderer& renderer);
    void BeginGravityInput();
    void SavePracticeCheckpoint();
    bool TryRespawnAtCheckpoint();
    bool UpdateTimer(float deltaTime);
    void UpdateMarioSprite(float deltaTime);
    void RenderSpriteInstance(Renderer& renderer, const SpriteInstance& instance);
    void RenderTiledSpriteInstance(Renderer& renderer, const TiledSpriteInstance& instance);
    bool IsVisibleInCamera(float x, float width) const;

    TextureManager& textureManager;
    SpriteManager spriteManager;
    FontManager fontManager;
    Camera camera;
    Mario mario;

    std::vector<SpriteInstance> backgroundSprites;
    std::vector<SpriteInstance> solidSprites;
    std::vector<TiledSpriteInstance> tiledSprites;
    std::vector<std::unique_ptr<QuestionBlock>> questionBlocks;
    std::vector<std::unique_ptr<Coin>> coins;
    std::vector<std::unique_ptr<Mushroom>> mushrooms;
    std::vector<std::unique_ptr<Star>> stars;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<FireballInstance> fireballs;
    std::vector<RectF> solidBounds;

    RectF winBounds{};
    float levelWidth = 0.0f;
    float levelHeight = 0.0f;
    float spawnX = 96.0f;
    float spawnY = 368.0f;
    float marioAnimationTime = 0.0f;
    int score = 0;
    int coinCount = 0;
    int timeLeft = 400;
    float timeAccumulator = 0.0f;
    bool assetsLoaded = false;
    std::wstring marioFontFamily;
    float lastDeltaTime = 0.0f;
    bool isPaused = false;
    int selectedPause = 0;
    bool debugMenuOpen = false;
    int debugMenuSelection = 0;
    bool debugGravityEditing = false;
    bool debugGravityInputInvalid = false;
    std::string debugGravityInput;
    bool noDeathsOnEnemyCollision = false;
    bool infiniteTime = false;
    bool practiceMode = false;
    PracticeCheckpoint practiceCheckpoint;
    float checkpointMessageTimer = 0.0f;
};
