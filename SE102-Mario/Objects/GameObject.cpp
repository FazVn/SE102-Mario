#include "GameObject.h"

float RectF::Width() const
{
    return right - left;
}

float RectF::Height() const
{
    return bottom - top;
}

bool RectF::Intersects(const RectF& other) const
{
    return left < other.right &&
        right > other.left &&
        top < other.bottom &&
        bottom > other.top;
}

GameObject::GameObject(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height)
{
}

void GameObject::Update(float deltaTime)
{
    x += velocityX * deltaTime;
    y += velocityY * deltaTime;
}

void GameObject::Render(Renderer&)
{
    // Base object does not know how it should look.
    // Derived classes such as Mario and item objects will override this.
}

void GameObject::OnCollision(GameObject&)
{
    // Default collision reaction: do nothing.
    // Derived classes can override this when needed.
}

float GameObject::GetX() const
{
    return x;
}

float GameObject::GetY() const
{
    return y;
}

float GameObject::GetWidth() const
{
    return width;
}

float GameObject::GetHeight() const
{
    return height;
}

float GameObject::GetVelocityX() const
{
    return velocityX;
}

float GameObject::GetVelocityY() const
{
    return velocityY;
}

void GameObject::SetPosition(float newX, float newY)
{
    x = newX;
    y = newY;
}

void GameObject::SetSize(float newWidth, float newHeight)
{
    width = newWidth < 0.0f ? 0.0f : newWidth;
    height = newHeight < 0.0f ? 0.0f : newHeight;
}

void GameObject::SetVelocity(float newVelocityX, float newVelocityY)
{
    velocityX = newVelocityX;
    velocityY = newVelocityY;
}

void GameObject::Move(float deltaX, float deltaY)
{
    x += deltaX;
    y += deltaY;
}

RectF GameObject::GetBoundingBox() const
{
    return RectF{ x, y, x + width, y + height };
}

bool GameObject::IsActive() const
{
    return active;
}

bool GameObject::IsVisible() const
{
    return active && visible;
}

bool GameObject::IsCollidable() const
{
    return active && collidable;
}

bool GameObject::IsSolid() const
{
    return active && collidable && solid;
}

void GameObject::SetActive(bool value)
{
    active = value;
}

void GameObject::SetVisible(bool value)
{
    visible = value;
}

void GameObject::SetCollidable(bool value)
{
    collidable = value;
}

void GameObject::SetSolid(bool value)
{
    solid = value;
}
