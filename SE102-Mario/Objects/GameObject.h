#pragma once

class Renderer;

struct RectF
{
    float left = 0.0f;
    float top = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;

    float Width() const;
    float Height() const;
    bool Intersects(const RectF& other) const;
};

class GameObject
{
public:
    GameObject() = default;
    GameObject(float x, float y, float width, float height);
    virtual ~GameObject() = default;

    virtual void Update(float deltaTime);
    virtual void Render(Renderer& renderer);
    virtual void OnCollision(GameObject& other);

    float GetX() const;
    float GetY() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetVelocityX() const;
    float GetVelocityY() const;

    void SetPosition(float newX, float newY);
    void SetSize(float newWidth, float newHeight);
    void SetVelocity(float newVelocityX, float newVelocityY);
    void Move(float deltaX, float deltaY);

    RectF GetBoundingBox() const;

    bool IsActive() const;
    bool IsVisible() const;
    bool IsCollidable() const;
    bool IsSolid() const;

    void SetActive(bool value);
    void SetVisible(bool value);
    void SetCollidable(bool value);
    void SetSolid(bool value);

protected:
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    float velocityX = 0.0f;
    float velocityY = 0.0f;

    bool active = true;
    bool visible = true;
    bool collidable = true;
    bool solid = false;
};
