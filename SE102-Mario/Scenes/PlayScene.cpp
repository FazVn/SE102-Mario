#include "PlayScene.h"

#include "../Core/AssetPaths.h"
#include "../Core/GameConfig.h"
#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "../Core/Sprite.h"
#include "../Objects/Enemies/Goomba.h"
#include "../Objects/Enemies/Koopa.h"
#include "SceneManager.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace
{
    constexpr int TileSize = 32;
    constexpr float MaxPhysicsDeltaTime = 0.05f;
    constexpr float MarioRenderWidth = 32.0f;
    constexpr float MarioRenderHeight = 48.0f;
    constexpr int InitialTime = 400;
    constexpr int MaximumScore = 999999;
    constexpr int CoinScore = 200;
    constexpr int EnemyScore = 100;
    constexpr int PowerUpScore = 1000;
    constexpr float EnemyActivationMargin = 96.0f;

    RectF MakeRect(float x, float y, float width, float height)
    {
        return RectF{ x, y, x + width, y + height };
    }

    std::string Trim(const std::string& value)
    {
        const auto begin = std::find_if_not(value.begin(), value.end(),
            [](unsigned char character) { return std::isspace(character) != 0; });
        const auto end = std::find_if_not(value.rbegin(), value.rend(),
            [](unsigned char character) { return std::isspace(character) != 0; }).base();

        if (begin >= end)
        {
            return std::string();
        }

        return std::string(begin, end);
    }

    std::string ToUpper(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char character) { return static_cast<char>(std::toupper(character)); });
        return value;
    }

    QuestionBlock::Content ParseQuestionBlockContent(const std::string& value)
    {
        const std::string upperValue = ToUpper(value);
        if (upperValue == "MUSHROOM")
        {
            return QuestionBlock::Content::Mushroom;
        }
        if (upperValue == "STAR")
        {
            return QuestionBlock::Content::Star;
        }

        return QuestionBlock::Content::Coin;
    }

    bool HasHorizontalOverlap(const RectF& first, const RectF& second)
    {
        return first.left < second.right && first.right > second.left;
    }

    std::wstring FormatScore(int score)
    {
        std::wostringstream stream;
        stream << std::setfill(L'0') << std::setw(6) << score;
        return stream.str();
    }
}

PlayScene::PlayScene(TextureManager& textures)
    : textureManager(textures)
{
}

void PlayScene::Load()
{
    score = 0;
    coinCount = 0;
    timeLeft = InitialTime;
    timeAccumulator = 0.0f;
    isPaused = false;
    selectedPause = 0;

    assetsLoaded = spriteManager.LoadFromDefinitionFile(L"definitions/sprites.txt", textureManager);

    if (fontManager.Load("mario", L"fonts/SS-Mario-Regular.otf", L"SS Mario"))
    {
        const std::wstring* loadedFamily = fontManager.GetFamily("mario");
        if (loadedFamily)
        {
            marioFontFamily = *loadedFamily;
        }
    }

    if (!LoadLevelFromFile(L"levels/level_1_1.txt"))
    {
        BuildFallbackLevel();
    }
}

void PlayScene::Unload()
{
    mario.SetSprite(nullptr);
    ClearLevel();
    spriteManager.Clear();
    fontManager.Clear();
}

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    lastDeltaTime = deltaTime;

    if (input.WasKeyPressed('G'))
    {
        sceneManager.RequestChange(SceneId::GameOver);
    }

    if (input.WasKeyPressed('R'))
    {
        sceneManager.RequestChange(SceneId::Play);
    }

    if (input.WasKeyPressed('H'))
        sceneManager.RequestChange(SceneId::Win);

    if (input.WasKeyPressed(VK_ESCAPE))
    {
        isPaused = !isPaused;
        selectedPause = 0;
        return;
    }

    if (isPaused)
    {
        if (input.WasKeyPressed(VK_UP))
        {
            selectedPause--;
            if (selectedPause < 0)
                selectedPause = 2;
        }

        if (input.WasKeyPressed(VK_DOWN))
        {
            selectedPause++;
            if (selectedPause > 2)
                selectedPause = 0;
        }

        if (input.WasKeyPressed(VK_RETURN))
        {
            if (selectedPause == 0)
            {
                isPaused = false; 
            }
            else if (selectedPause == 1)
            {
                sceneManager.RequestChange(SceneId::Menu);
            }
            else
            {
                PostQuitMessage(0);
            }
        }

        return; 
    }

    if (UpdateTimer(deltaTime))
    {
        sceneManager.RequestChange(SceneId::GameOver);
        return;
    }

    const float physicsDeltaTime = deltaTime > MaxPhysicsDeltaTime ? MaxPhysicsDeltaTime : deltaTime;
    mario.Update(input, physicsDeltaTime);
    UpdateQuestionBlocks(physicsDeltaTime);
    mario.ResolveSolidCollisions(solidBounds);

    if (mario.GetX() < 0.0f)
    {
        mario.SetPosition(0.0f, mario.GetY());
    }

    if (mario.GetX() + mario.GetWidth() > levelWidth)
    {
        mario.SetPosition(levelWidth - mario.GetWidth(), mario.GetY());
    }

    if (UpdateEnemies(physicsDeltaTime))
    {
        sceneManager.RequestChange(SceneId::GameOver);
        return;
    }

    UpdateItems(physicsDeltaTime);

    if (mario.GetY() > levelHeight + 128.0f)
    {
        sceneManager.RequestChange(SceneId::GameOver);
        return;
    }

    if (winBounds.Width() > 0.0f && winBounds.Height() > 0.0f && mario.GetBoundingBox().Intersects(winBounds))
    {
        AwardTimeBonus();
        sceneManager.RequestChange(SceneId::Win);
        return;
    }

    UpdateCamera();
    UpdateMarioSprite(deltaTime);
}

void PlayScene::Render(Renderer& renderer, HWND windowHandle)
{
    renderer.Begin(windowHandle, RGB(92, 148, 252));

    for (const SpriteInstance& sprite : backgroundSprites)
    {
        RenderSpriteInstance(renderer, sprite);
    }

    for (const TiledSpriteInstance& sprite : tiledSprites)
    {
        RenderTiledSpriteInstance(renderer, sprite);
    }

    for (const SpriteInstance& sprite : solidSprites)
    {
        RenderSpriteInstance(renderer, sprite);
    }

    for (const auto& block : questionBlocks)
    {
        if (block && IsVisibleInCamera(block->GetX(), block->GetWidth()))
        {
            block->RenderAt(renderer, camera.GetX(), camera.GetY());
        }
    }

    for (const auto& coin : coins)
    {
        if (coin && IsVisibleInCamera(coin->GetX(), coin->GetWidth()))
        {
            coin->RenderAt(renderer, camera.GetX(), camera.GetY());
        }
    }

    for (const auto& mushroom : mushrooms)
    {
        if (mushroom && IsVisibleInCamera(mushroom->GetX(), mushroom->GetWidth()))
        {
            mushroom->RenderAt(renderer, camera.GetX(), camera.GetY());
        }
    }

    for (const auto& star : stars)
    {
        if (star && IsVisibleInCamera(star->GetX(), star->GetWidth()))
        {
            star->RenderAt(renderer, camera.GetX(), camera.GetY());
        }
    }

    for (const auto& enemy : enemies)
    {
        if (!enemy || !enemy->IsActive() || !IsVisibleInCamera(enemy->GetX(), enemy->GetWidth()))
        {
            continue;
        }

        enemy->RenderAt(renderer, camera.GetX(), camera.GetY());
    }


    mario.RenderAt(renderer, camera.GetX(), camera.GetY());

    renderer.DrawTextLine(FormatScore(score), 24, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);
    renderer.DrawTextLine(L"COIN " + std::to_wstring(coinCount), 210, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);
    renderer.DrawTextLine(L"WORLD 1-1", 400, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);
    renderer.DrawTextLine(L"TIME " + std::to_wstring(timeLeft), 680, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);

    if (isPaused)
    {
        renderer.DrawCenteredText(
            L"PAUSED", 80, 60, 42, RGB(255, 255, 255));

        if (selectedPause == 0)
            renderer.DrawCenteredText(L"> RESUME <", 170, 40, 28, RGB(255, 230, 80));
        else
            renderer.DrawCenteredText(L"RESUME", 170, 40, 28, RGB(180, 180, 180));

        if (selectedPause == 1)
            renderer.DrawCenteredText(L"> RETURN TO MENU <", 220, 40, 28, RGB(255, 230, 80));
        else
            renderer.DrawCenteredText(L"RETURN TO MENU", 220, 40, 28, RGB(180, 180, 180));

        if (selectedPause == 2)
            renderer.DrawCenteredText(L"> EXIT <", 270, 40, 28, RGB(255, 230, 80));
        else
            renderer.DrawCenteredText(L"EXIT", 270, 40, 28, RGB(180, 180, 180));
    }


    renderer.End();
}

bool PlayScene::LoadLevelFromFile(const std::wstring& resourceRelativePath)
{
    ClearLevel();

    const std::filesystem::path levelPath(AssetPaths::ResolveResourcePath(resourceRelativePath));
    std::ifstream file(levelPath);
    if (!file)
    {
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        const size_t commentPosition = line.find('#');
        if (commentPosition != std::string::npos)
        {
            line = line.substr(0, commentPosition);
        }

        line = Trim(line);
        if (line.empty() || line.front() == '[')
        {
            continue;
        }

        std::istringstream stream(line);
        std::string command;
        stream >> command;
        command = ToUpper(command);

        if (command == "WIDTH")
        {
            stream >> levelWidth;
        }
        else if (command == "HEIGHT")
        {
            stream >> levelHeight;
        }
        else if (command == "START_X")
        {
            stream >> spawnX;
        }
        else if (command == "START_Y")
        {
            stream >> spawnY;
        }
        else if (command == "GROUND")
        {
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            if (stream >> x >> y >> width >> height)
            {
                AddTiledSprite("block.dirt", x, y, width, height, TileSize, TileSize, true);
            }
        }
        else if (command == "TILE" || command == "SOLID_TILE")
        {
            std::string spriteId;
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            int tileWidth = TileSize;
            int tileHeight = TileSize;
            if (stream >> spriteId >> x >> y >> width >> height)
            {
                stream >> tileWidth >> tileHeight;
                AddTiledSprite(spriteId, x, y, width, height, tileWidth, tileHeight, command == "SOLID_TILE");
            }
        }
        else if (command == "SPRITE" || command == "BACKGROUND")
        {
            std::string spriteId;
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            if (stream >> spriteId >> x >> y >> width >> height)
            {
                AddSprite(spriteId, x, y, width, height);
            }
        }
        else if (command == "SOLID_SPRITE")
        {
            std::string spriteId;
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            if (stream >> spriteId >> x >> y >> width >> height)
            {
                AddSolidSprite(spriteId, x, y, width, height);
            }
        }
        else if (command == "BRICK")
        {
            float x = 0.0f;
            float y = 0.0f;
            std::string spriteId = "block.gold.tanborder.brick";
            if (stream >> x >> y)
            {
                stream >> spriteId;
                AddSolidSprite(spriteId, x, y, TileSize, TileSize);
            }
        }
        else if (command == "QUESTION_BLOCK")
        {
            float x = 0.0f;
            float y = 0.0f;
            if (stream >> x >> y)
            {
                std::string contentName = "COIN";
                stream >> contentName;
                AddQuestionBlock(x, y, ParseQuestionBlockContent(contentName));
            }
        }
        else if (command == "USED_BLOCK")
        {
            float x = 0.0f;
            float y = 0.0f;
            if (stream >> x >> y)
            {
                AddSolidSprite("block.used", x, y, TileSize, TileSize);
            }
        }
        else if (command == "PIPE")
        {
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            if (stream >> x >> y >> width >> height)
            {
                AddPipe(x, y, width, height);
            }
        }
        else if (command == "STAIR_RIGHT" || command == "STAIR_LEFT")
        {
            float x = 0.0f;
            float y = 0.0f;
            int steps = 0;
            if (stream >> x >> y >> steps)
            {
                AddStaircase(x, y, steps, command == "STAIR_RIGHT" ? 1 : -1);
            }
        }
        else if (command == "GOOMBA" || command == "KOOPA")
        {
            float x = 0.0f;
            float y = 0.0f;
            if (stream >> x >> y)
            {
                AddEnemy(command, x, y);
            }
        }
        else if (command == "COIN")
        {
            float x = 0.0f;
            float y = 0.0f;
            if (stream >> x >> y)
            {
                AddCoin(x, y);
            }
        }
        else if (command == "WIN")
        {
            float x = 0.0f;
            float y = 0.0f;
            float width = 0.0f;
            float height = 0.0f;
            if (stream >> x >> y >> width >> height)
            {
                winBounds = MakeRect(x, y, width, height);
            }
        }
    }

    levelWidth = levelWidth > static_cast<float>(GameConfig::WindowWidth) ? levelWidth : static_cast<float>(GameConfig::WindowWidth);
    levelHeight = levelHeight > static_cast<float>(GameConfig::WindowHeight) ? levelHeight : static_cast<float>(GameConfig::WindowHeight);

    mario.SetSprite(spriteManager.Get("mario.super.stand"));
    mario.SetRenderSize(static_cast<int>(MarioRenderWidth), static_cast<int>(MarioRenderHeight));
    mario.SetPosition(spawnX, spawnY);
    mario.SetOnGround(true);
    UpdateCamera();
    UpdateMarioSprite(0.0f);
    return true;
}

void PlayScene::BuildFallbackLevel()
{
    ClearLevel();

    levelWidth = static_cast<float>(GameConfig::WindowWidth);
    levelHeight = static_cast<float>(GameConfig::WindowHeight);
    spawnX = 96.0f;
    spawnY = 368.0f;

    AddTiledSprite("block.dirt", 0.0f, 416.0f, static_cast<float>(GameConfig::WindowWidth), 64.0f, TileSize, TileSize, true);
    AddTiledSprite("block.dirt", 320.0f, 384.0f, 160.0f, 32.0f, TileSize, TileSize, true);

    mario.SetSprite(spriteManager.Get("mario.super.stand"));
    mario.SetRenderSize(static_cast<int>(MarioRenderWidth), static_cast<int>(MarioRenderHeight));
    mario.SetPosition(spawnX, spawnY);
    mario.SetOnGround(true);
    UpdateCamera();
}

void PlayScene::ClearLevel()
{
    backgroundSprites.clear();
    solidSprites.clear();
    tiledSprites.clear();
    questionBlocks.clear();
    coins.clear();
    mushrooms.clear();
    stars.clear();
    enemies.clear();
    solidBounds.clear();
    winBounds = RectF{};
    levelWidth = static_cast<float>(GameConfig::WindowWidth);
    levelHeight = static_cast<float>(GameConfig::WindowHeight);
    spawnX = 96.0f;
    spawnY = 368.0f;
    marioAnimationTime = 0.0f;
    camera.Follow(0.0f, 0.0f);
}

void PlayScene::AddSolidRect(float x, float y, float width, float height)
{
    if (width <= 0.0f || height <= 0.0f)
    {
        return;
    }

    solidBounds.push_back(MakeRect(x, y, width, height));
}

void PlayScene::AddSprite(const std::string& spriteId, float x, float y, float width, float height)
{
    backgroundSprites.push_back(SpriteInstance{ spriteManager.Get(spriteId), x, y, width, height });
}

void PlayScene::AddSolidSprite(const std::string& spriteId, float x, float y, float width, float height)
{
    solidSprites.push_back(SpriteInstance{ spriteManager.Get(spriteId), x, y, width, height });
    AddSolidRect(x, y, width, height);
}

void PlayScene::AddTiledSprite(const std::string& spriteId, float x, float y, float width, float height, int tileWidth, int tileHeight, bool solid)
{
    tiledSprites.push_back(TiledSpriteInstance{
        spriteManager.Get(spriteId),
        x,
        y,
        width,
        height,
        tileWidth > 0 ? tileWidth : TileSize,
        tileHeight > 0 ? tileHeight : TileSize
        });

    if (solid)
    {
        AddSolidRect(x, y, width, height);
    }
}

void PlayScene::AddPipe(float x, float y, float width, float height)
{
    if (width <= 0.0f || height <= 0.0f)
    {
        return;
    }

    AddSolidRect(x, y, width, height);
    solidSprites.push_back(SpriteInstance{ spriteManager.Get("pipe.green.top"), x, y, width, TileSize });

    const float bodyHeight = height - static_cast<float>(TileSize);
    if (bodyHeight > 0.0f)
    {
        tiledSprites.push_back(TiledSpriteInstance{
            spriteManager.Get("pipe.green.body"),
            x,
            y + static_cast<float>(TileSize),
            width,
            bodyHeight,
            static_cast<int>(std::lround(width)),
            TileSize
            });
    }
}

void PlayScene::AddStaircase(float x, float y, int steps, int direction)
{
    const int safeSteps = std::max(0, steps);
    const int safeDirection = direction < 0 ? -1 : 1;

    for (int column = 0; column < safeSteps; ++column)
    {
        const float blockX = x + static_cast<float>(safeDirection * column * TileSize);
        for (int row = 0; row <= column; ++row)
        {
            const float blockY = y - static_cast<float>(row * TileSize);
            AddSolidSprite("block.gold.tanborder.brick", blockX, blockY, TileSize, TileSize);
        }
    }
}

void PlayScene::AddEnemy(const std::string& enemyType, float x, float y)
{
    if (enemyType == "KOOPA")
    {
        enemies.push_back(std::make_unique<Koopa>(
            x,
            y,
            spriteManager.Get("enemy.koopa.walk1"),
            spriteManager.Get("enemy.koopa.walk2")));
    }
    else
    {
        enemies.push_back(std::make_unique<Goomba>(
            x,
            y,
            spriteManager.Get("enemy.goomba.walk1"),
            spriteManager.Get("enemy.goomba.walk2")));
    }
}

void PlayScene::AddCoin(float x, float y)
{
    coins.push_back(std::make_unique<Coin>(
        x,
        y,
        spriteManager.Get("item.coin.frame1"),
        spriteManager.Get("item.coin.frame2"),
        spriteManager.Get("item.coin.frame3"),
        spriteManager.Get("item.coin.frame4")));
}

void PlayScene::AddQuestionBlock(float x, float y, QuestionBlock::Content content)
{
    questionBlocks.push_back(std::make_unique<QuestionBlock>(
        x,
        y,
        spriteManager.Get("block.question.frame1"),
        spriteManager.Get("block.question.frame2"),
        spriteManager.Get("block.question.frame3"),
        spriteManager.Get("block.used"),
        content));
    AddSolidRect(x, y, TileSize, TileSize);
}

void PlayScene::SpawnQuestionBlockContent(const QuestionBlock& block)
{
    switch (block.GetContent())
    {
    case QuestionBlock::Content::Mushroom:
        mushrooms.push_back(std::make_unique<Mushroom>(
            block.GetX(),
            block.GetY() - 32.0f,
            spriteManager.Get("item.mushroom.red")));
        break;

    case QuestionBlock::Content::Star:
        stars.push_back(std::make_unique<Star>(
            block.GetX(),
            block.GetY() - 32.0f,
            spriteManager.Get("item.star.yellow"),
            spriteManager.Get("item.star.green"),
            spriteManager.Get("item.star.red")));
        break;

    case QuestionBlock::Content::Coin:
    default:
        coins.push_back(std::make_unique<Coin>(
            block.GetX() + 4.0f,
            block.GetY() - 24.0f,
            spriteManager.Get("item.coin.frame1"),
            spriteManager.Get("item.coin.frame2"),
            spriteManager.Get("item.coin.frame3"),
            spriteManager.Get("item.coin.frame4"),
            true));
        AddCoin();
        break;
    }
}

void PlayScene::AddScore(int amount)
{
    if (amount <= 0 || score >= MaximumScore)
    {
        return;
    }

    score = amount > MaximumScore - score ? MaximumScore : score + amount;
}

void PlayScene::AddCoin()
{
    ++coinCount;
    AddScore(CoinScore);
}

void PlayScene::OnEnemyDefeated(Enemy& enemy)
{
    if (!enemy.IsActive())
    {
        return;
    }

    enemy.Defeat();
    AddScore(EnemyScore);
}

void PlayScene::AwardTimeBonus()
{
    int bonus = 0;
    for (int second = timeLeft; second > 0; --second)
    {
        if (second >= 350)
        {
            bonus += 250;
        }
        else if (second >= 300)
        {
            bonus += 80;
        }
        else if (second >= 200)
        {
            bonus += 20;
        }
        else
        {
            bonus += 5;
        }
    }

    AddScore(bonus);
}

void PlayScene::UpdateCamera()
{
    const float diff = levelWidth - static_cast<float>(GameConfig::WindowWidth);
    const float maxCameraX = diff > 0.0f ? diff : 0.0f;

    const float targetX = mario.GetX() + mario.GetWidth() * 0.5f - static_cast<float>(GameConfig::WindowWidth) * 0.42f;
    camera.Follow(std::clamp(targetX, 0.0f, maxCameraX), 0.0f);
}

bool PlayScene::UpdateEnemies(float deltaTime)
{
    bool hitMario = false;

    for (const auto& enemy : enemies)
    {
        if (!enemy || !enemy->IsActive())
        {
            continue;
        }

        const float activationRight = camera.GetX() +
            static_cast<float>(GameConfig::WindowWidth) + EnemyActivationMargin;
        if (enemy->GetX() > activationRight)
        {
            continue;
        }

        enemy->Update(deltaTime, solidBounds, levelHeight);

        const RectF enemyBounds = enemy->GetBoundingBox();
        const RectF marioBounds = mario.GetBoundingBox();
        if (marioBounds.Intersects(enemyBounds))
        {
            if (mario.IsInvincible())
            {
                OnEnemyDefeated(*enemy);
                continue;
            }

            const bool stomped = mario.GetVelocityY() > 0.0f && marioBounds.bottom <= enemyBounds.top + 16.0f;
            if (stomped)
            {
                OnEnemyDefeated(*enemy);
                mario.Bounce();
            }
            else
            {
                hitMario = true;
            }
        }
    }

    return hitMario;
}

void PlayScene::UpdateQuestionBlocks(float deltaTime)
{
    const RectF previousMarioBounds = mario.GetPreviousBoundingBox();
    const RectF marioBounds = mario.GetBoundingBox();
    const bool marioMovingUp = mario.GetVelocityY() < 0.0f;

    for (const auto& block : questionBlocks)
    {
        if (!block)
        {
            continue;
        }

        block->Update(deltaTime);

        if (!marioMovingUp || block->IsUsed())
        {
            continue;
        }

        const RectF blockBounds = block->GetBoundingBox();
        const bool crossedBlockBottom = previousMarioBounds.top >= blockBounds.bottom &&
            marioBounds.top <= blockBounds.bottom;

        if (crossedBlockBottom && HasHorizontalOverlap(marioBounds, blockBounds))
        {
            if (block->Hit())
            {
                SpawnQuestionBlockContent(*block);
            }
        }
    }
}

void PlayScene::UpdateItems(float deltaTime)
{
    const RectF marioBounds = mario.GetBoundingBox();

    for (const auto& coin : coins)
    {
        if (!coin || !coin->IsActive())
        {
            continue;
        }

        coin->Update(deltaTime);

        if (coin->IsCollidable() && marioBounds.Intersects(coin->GetBoundingBox()))
        {
            coin->SetActive(false);
            AddCoin();
        }
    }

    for (const auto& mushroom : mushrooms)
    {
        if (!mushroom || !mushroom->IsActive())
        {
            continue;
        }

        mushroom->Update(deltaTime, solidBounds, levelHeight);
        if (marioBounds.Intersects(mushroom->GetBoundingBox()))
        {
            mushroom->SetActive(false);
            mario.SetRenderSize(static_cast<int>(MarioRenderWidth), static_cast<int>(MarioRenderHeight));
            AddScore(PowerUpScore);
        }
    }

    for (const auto& star : stars)
    {
        if (!star || !star->IsActive())
        {
            continue;
        }

        star->Update(deltaTime, solidBounds, levelHeight);
        if (marioBounds.Intersects(star->GetBoundingBox()))
        {
            star->SetActive(false);
            mario.ActivateStarPower();
            AddScore(PowerUpScore);
        }
    }

    coins.erase(std::remove_if(coins.begin(), coins.end(),
        [](const std::unique_ptr<Coin>& coin) { return !coin || coin->IsFinished(); }),
        coins.end());

    mushrooms.erase(std::remove_if(mushrooms.begin(), mushrooms.end(),
        [](const std::unique_ptr<Mushroom>& mushroom) { return !mushroom || !mushroom->IsActive(); }),
        mushrooms.end());

    stars.erase(std::remove_if(stars.begin(), stars.end(),
        [](const std::unique_ptr<Star>& star) { return !star || !star->IsActive(); }),
        stars.end());
}

bool PlayScene::UpdateTimer(float deltaTime)
{
    if (timeLeft <= 0)
    {
        return true;
    }

    if (deltaTime > 0.0f)
    {
        timeAccumulator += deltaTime;
    }

    while (timeAccumulator >= 1.0f && timeLeft > 0)
    {
        timeAccumulator -= 1.0f;
        --timeLeft;
    }

    return timeLeft <= 0;
}

void PlayScene::UpdateMarioSprite(float deltaTime)
{
    marioAnimationTime += deltaTime;

    std::string spriteId = "mario.super.stand";
    if (mario.GetState() == Mario::State::Jump || mario.GetState() == Mario::State::Fall)
    {
        spriteId = "mario.super.jump";
    }
    else if (mario.GetState() == Mario::State::Walk)
    {
        const int frame = static_cast<int>(marioAnimationTime / 0.08f) % 3;
        if (frame == 0)
        {
            spriteId = "mario.super.walk1";
        }
        else if (frame == 1)
        {
            spriteId = "mario.super.walk2";
        }
        else
        {
            spriteId = "mario.super.walk3";
        }
    }

    mario.SetSprite(spriteManager.Get(spriteId));
}

void PlayScene::RenderSpriteInstance(Renderer& renderer, const SpriteInstance& instance)
{
    if (!instance.sprite || instance.width <= 0.0f || instance.height <= 0.0f || !IsVisibleInCamera(instance.x, instance.width))
    {
        return;
    }

    renderer.DrawSprite(*instance.sprite,
        static_cast<int>(std::lround(instance.x - camera.GetX())),
        static_cast<int>(std::lround(instance.y - camera.GetY())),
        static_cast<int>(std::lround(instance.width)),
        static_cast<int>(std::lround(instance.height)));
}

void PlayScene::RenderTiledSpriteInstance(Renderer& renderer, const TiledSpriteInstance& instance)
{
    if (!instance.sprite || instance.tileWidth <= 0 || instance.tileHeight <= 0 || !IsVisibleInCamera(instance.x, instance.width))
    {
        return;
    }

    const int drawX = static_cast<int>(std::lround(instance.x - camera.GetX()));
    const int drawY = static_cast<int>(std::lround(instance.y - camera.GetY()));
    const int drawWidth = static_cast<int>(std::lround(instance.width));
    const int drawHeight = static_cast<int>(std::lround(instance.height));

    for (int yOffset = 0; yOffset + instance.tileHeight <= drawHeight; yOffset += instance.tileHeight)
    {
        for (int xOffset = 0; xOffset + instance.tileWidth <= drawWidth; xOffset += instance.tileWidth)
        {
            renderer.DrawSprite(*instance.sprite, drawX + xOffset, drawY + yOffset, instance.tileWidth, instance.tileHeight);
        }
    }
}

bool PlayScene::IsVisibleInCamera(float x, float width) const
{
    const float cameraX = camera.GetX();
    return x + width >= cameraX - 96.0f &&
        x <= cameraX + static_cast<float>(GameConfig::WindowWidth) + 96.0f;
}
