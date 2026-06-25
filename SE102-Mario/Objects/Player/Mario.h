#pragma once

#include "../GameObject.h"

#include <vector>

class Input;
class Sprite;

class Mario : public GameObject
{
public:
    enum class FacingDirection
    {
        Left = -1,
        Right = 1
    };

    enum class State
    {
        Idle,
        Walk,
        Jump,
        Fall
    };

    static constexpr int SmallStandXLeft = 3;
    static constexpr int SmallStandYTop = 35;
    static constexpr int SmallStandXRight = 22;
    static constexpr int SmallStandYDown = 65;
    static constexpr int DefaultRenderWidth = 19;
    static constexpr int DefaultRenderHeight = 30;

    static constexpr float PixelToMeter = 31.0f / 600.0f;
    static constexpr float MeterToPixel = 600.0f / 31.0f;
    static constexpr float Gravity = 9.81f;
    static constexpr float MaxRunSpeed = 6.8f;
    static constexpr float RunAcceleration = 3.0f;
    static constexpr float RunDeceleration = 3.0f;
    static constexpr float JumpHeightMultiplier = 1.5f;

    Mario();
    Mario(float x, float y, const Sprite* sprite, int renderWidth = DefaultRenderWidth, int renderHeight = DefaultRenderHeight);

    void SetSprite(const Sprite* sprite);
    void SetRenderSize(int renderWidth, int renderHeight);
    void SetPosition(float newX, float newY);
    void SetOnGround(bool value);

    void Update(const Input& input, float deltaTime);
    void ResolveSolidCollisions(const std::vector<RectF>& solidBounds);
    void ResolveVerticalCollision(const std::vector<RectF>& solidBounds);
    void Render(Renderer& renderer) override;

    bool IsOnGround() const;
    FacingDirection GetFacingDirection() const;
    State GetState() const;

private:
    void SyncPixelsFromPhysics();
    void SyncPhysicsFromPixels();
    void UpdateState();

    const Sprite* sprite = nullptr;
    int renderWidth = DefaultRenderWidth;
    int renderHeight = DefaultRenderHeight;

    float positionMetersX = 0.0f;
    float positionMetersY = 0.0f;
    float velocityMetersX = 0.0f;
    float velocityMetersY = 0.0f;
    float previousPixelX = 0.0f;
    float previousPixelY = 0.0f;

    FacingDirection facingDirection = FacingDirection::Right;
    State state = State::Idle;
    bool onGround = false;
};
