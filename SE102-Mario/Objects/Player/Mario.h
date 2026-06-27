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

    enum class Form
    {
        Small,
        Super,
        Fire
    };

    enum class Transformation
    {
        None,
        SmallToSuper,
        SuperToFire,
        FireToSuper,
        SuperToSmall
    };

    static constexpr int SmallStandXLeft = 3;
    static constexpr int SmallStandYTop = 35;
    static constexpr int SmallStandXRight = 22;
    static constexpr int SmallStandYDown = 65;
    static constexpr int SmallRenderWidth = 32;
    static constexpr int SmallRenderHeight = 32;
    static constexpr int PoweredRenderWidth = 32;
    static constexpr int PoweredRenderHeight = 48;
    static constexpr int DefaultRenderWidth = SmallRenderWidth;
    static constexpr int DefaultRenderHeight = SmallRenderHeight;

    static constexpr float PixelToMeter = 31.0f / 600.0f;
    static constexpr float MeterToPixel = 600.0f / 31.0f;
    static constexpr float DefaultGravity = 9.81f;
    static constexpr float MaxRunSpeed = 6.8f;
    static constexpr float RunAcceleration = 3.0f;
    static constexpr float RunDeceleration = 3.0f;
    static constexpr float JumpHeightMultiplier = 3.25f;
    static constexpr float StarPowerDuration = 10.0f;
    static constexpr float StarPowerSpeedMultiplier = 1.45f;
    static constexpr float TransformationDuration = 0.6f;
    static constexpr float DamageInvulnerabilityDuration = 1.5f;
    static constexpr float ShootAnimationDuration = 0.18f;

    Mario();
    Mario(float x, float y, const Sprite* sprite, int renderWidth = DefaultRenderWidth, int renderHeight = DefaultRenderHeight);

    void SetSprite(const Sprite* sprite);
    void SetRenderSize(int renderWidth, int renderHeight);
    void SetPosition(float newX, float newY);
    void SetOnGround(bool value);
    void Bounce();
    void ResetPowerState();
    void SetPowerForm(Form newForm);
    void SetGravity(float newGravity);
    void Respawn(float newX, float newY, Form newForm, bool wasOnGround);
    void UpgradeWithMushroom();
    bool TakeHit();
    bool CanShootFireball() const;
    void StartShootAnimation();

    void Update(const Input& input, float deltaTime);
    void ResolveSolidCollisions(const std::vector<RectF>& solidBounds);
    void ResolveVerticalCollision(const std::vector<RectF>& solidBounds);
    void Render(Renderer& renderer) override;
    void RenderAt(Renderer& renderer, float offsetX, float offsetY);

    bool IsOnGround() const;
    bool IsInvincible() const;
    bool IsDamageImmune() const;
    bool IsShooting() const;
    bool IsTransforming() const;
    RectF GetPreviousBoundingBox() const;
    FacingDirection GetFacingDirection() const;
    State GetState() const;
    Form GetPowerForm() const;
    float GetGravity() const;
    Transformation GetTransformation() const;
    float GetTransformationProgress() const;
    void ActivateStarPower();

private:
    void SyncPixelsFromPhysics();
    void SyncPhysicsFromPixels();
    void UpdateState();
    void ApplyForm(Form newForm);
    void StartTransformation(Transformation newTransformation);

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
    Form form = Form::Small;
    Transformation transformation = Transformation::None;
    bool onGround = false;
    float starPowerTimer = 0.0f;
    float transformationTimer = 0.0f;
    float damageInvulnerabilityTimer = 0.0f;
    float shootAnimationTimer = 0.0f;
    float gravity = DefaultGravity;
};
