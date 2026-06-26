#include "PlayScene.h"

#include "../Core/AssetPaths.h"
#include "../Core/GameConfig.h"
#include "../Core/Input.h"
#include "../Core/Renderer.h"
#include "../Core/Sprite.h"
#include "SceneManager.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
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
    constexpr float EnemyGravityPixels = 980.0f;
    constexpr float MarioRenderWidth = 32.0f;
    constexpr float MarioRenderHeight = 48.0f;

    RectF MakeRect(float x, float y, float width, float height)
    {
        return RectF{ x, y, x + width, y + height };
    }

    bool HasHorizontalOverlap(const RectF& first, const RectF& second)
    {
        return first.left < second.right && first.right > second.left;
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
}

void PlayScene::Load()
{
    assetsLoaded = textureManager.LoadFromDefinitionFile(L"definitions/textures.txt");
    assetsLoaded = spriteManager.LoadFromDefinitionFile(L"definitions/sprites.txt", textureManager) && assetsLoaded;

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
    textureManager.Clear();
    fontManager.Clear();
}

void PlayScene::Update(SceneManager& sceneManager, const Input& input, float deltaTime)
{
    if (input.WasKeyPressed('G'))
    {
        sceneManager.RequestChange(SceneId::GameOver);
    }

    if (input.WasKeyPressed('R'))
    {
        sceneManager.RequestChange(SceneId::Play);
    }

    const float physicsDeltaTime = deltaTime > MaxPhysicsDeltaTime ? MaxPhysicsDeltaTime : deltaTime;
    mario.Update(input, physicsDeltaTime);
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

    UpdateCoins();

    if (mario.GetY() > levelHeight + 128.0f)
    {
        sceneManager.RequestChange(SceneId::GameOver);
        return;
    }

    if (winBounds.Width() > 0.0f && winBounds.Height() > 0.0f && mario.GetBoundingBox().Intersects(winBounds))
    {
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

    for (const CoinInstance& coin : coins)
    {
        if (coin.active)
        {
            RenderSpriteInstance(renderer, coin.visual);
        }
    }

    for (const EnemyInstance& enemy : enemies)
    {
        if (!enemy.active || !IsVisibleInCamera(enemy.x, enemy.width))
        {
            continue;
        }

        const Sprite* frame = (static_cast<int>(enemy.animationTime / 0.16f) % 2 == 0)
            ? enemy.walkFrame1
            : enemy.walkFrame2;
        if (!frame)
        {
            frame = enemy.walkFrame1 ? enemy.walkFrame1 : enemy.walkFrame2;
        }

        if (frame)
        {
            renderer.DrawSprite(*frame,
                static_cast<int>(std::lround(enemy.x - camera.GetX())),
                static_cast<int>(std::lround(enemy.y - camera.GetY())),
                static_cast<int>(std::lround(enemy.width)),
                static_cast<int>(std::lround(enemy.height)));
        }
    }

    mario.RenderAt(renderer, camera.GetX(), camera.GetY());

    renderer.DrawTextLine(L"WORLD 1-1", 24, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);
    renderer.DrawTextLine(L"COIN " + std::to_wstring(coinsCollected), 300, 14, 20, RGB(255, 255, 255), marioFontFamily.c_str(), FW_BOLD);

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
                AddSolidSprite("block.question.frame1", x, y, TileSize, TileSize);
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

    levelWidth = std::max(levelWidth, static_cast<float>(GameConfig::WindowWidth));
    levelHeight = std::max(levelHeight, static_cast<float>(GameConfig::WindowHeight));

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
    coins.clear();
    enemies.clear();
    solidBounds.clear();
    winBounds = RectF{};
    levelWidth = static_cast<float>(GameConfig::WindowWidth);
    levelHeight = static_cast<float>(GameConfig::WindowHeight);
    spawnX = 96.0f;
    spawnY = 368.0f;
    marioAnimationTime = 0.0f;
    coinsCollected = 0;
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
    EnemyInstance enemy;
    enemy.x = x;
    enemy.y = y;

    if (enemyType == "KOOPA")
    {
        enemy.walkFrame1 = spriteManager.Get("enemy.koopa.walk1");
        enemy.walkFrame2 = spriteManager.Get("enemy.koopa.walk2");
        enemy.width = 32.0f;
        enemy.height = 48.0f;
        enemy.velocityX = -36.0f;
    }
    else
    {
        enemy.walkFrame1 = spriteManager.Get("enemy.goomba.walk1");
        enemy.walkFrame2 = spriteManager.Get("enemy.goomba.walk2");
        enemy.width = 32.0f;
        enemy.height = 32.0f;
        enemy.velocityX = -42.0f;
    }

    enemies.push_back(enemy);
}

void PlayScene::AddCoin(float x, float y)
{
    coins.push_back(CoinInstance{ SpriteInstance{ spriteManager.Get("item.coin.frame1"), x, y, 24.0f, 24.0f }, true });
}

void PlayScene::UpdateCamera()
{
    const float maxCameraX = std::max(0.0f, levelWidth - static_cast<float>(GameConfig::WindowWidth));
    const float targetX = mario.GetX() + mario.GetWidth() * 0.5f - static_cast<float>(GameConfig::WindowWidth) * 0.42f;
    camera.Follow(std::clamp(targetX, 0.0f, maxCameraX), 0.0f);
}

bool PlayScene::UpdateEnemies(float deltaTime)
{
    bool hitMario = false;

    for (EnemyInstance& enemy : enemies)
    {
        if (!enemy.active)
        {
            continue;
        }

        enemy.animationTime += deltaTime;
        enemy.velocityY += EnemyGravityPixels * deltaTime;

        const float previousX = enemy.x;
        enemy.x += enemy.velocityX * deltaTime;
        RectF enemyBounds = MakeRect(enemy.x, enemy.y, enemy.width, enemy.height);

        for (const RectF& solid : solidBounds)
        {
            if (enemyBounds.Intersects(solid))
            {
                enemy.x = previousX;
                enemy.velocityX = -enemy.velocityX;
                break;
            }
        }

        const float previousY = enemy.y;
        enemy.y += enemy.velocityY * deltaTime;
        enemyBounds = MakeRect(enemy.x, enemy.y, enemy.width, enemy.height);

        bool landed = false;
        float landingTop = 0.0f;
        if (enemy.velocityY >= 0.0f)
        {
            for (const RectF& solid : solidBounds)
            {
                const bool crossedTop = previousY + enemy.height <= solid.top && enemyBounds.bottom >= solid.top;
                if (HasHorizontalOverlap(enemyBounds, solid) && crossedTop)
                {
                    if (!landed || solid.top < landingTop)
                    {
                        landed = true;
                        landingTop = solid.top;
                    }
                }
            }
        }

        if (landed)
        {
            enemy.y = landingTop - enemy.height;
            enemy.velocityY = 0.0f;
        }

        if (enemy.y > levelHeight + 128.0f)
        {
            enemy.active = false;
            continue;
        }

        enemyBounds = MakeRect(enemy.x, enemy.y, enemy.width, enemy.height);
        const RectF marioBounds = mario.GetBoundingBox();
        if (marioBounds.Intersects(enemyBounds))
        {
            const bool stomped = mario.GetVelocityY() > 0.0f && marioBounds.bottom <= enemyBounds.top + 16.0f;
            if (stomped)
            {
                enemy.active = false;
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

void PlayScene::UpdateCoins()
{
    const RectF marioBounds = mario.GetBoundingBox();

    for (CoinInstance& coin : coins)
    {
        if (!coin.active)
        {
            continue;
        }

        if (marioBounds.Intersects(MakeRect(coin.visual.x, coin.visual.y, coin.visual.width, coin.visual.height)))
        {
            coin.active = false;
            ++coinsCollected;
        }
    }
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
