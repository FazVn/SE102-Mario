#include "Renderer.h"

#include "Sprite.h"

void Renderer::Initialize(
    ID3D10Device* device,
    IDXGISwapChain* swapChain,
    ID3D10RenderTargetView* renderTargetView,
    ID3DX10Sprite* spriteHandler,
    ID3D10BlendState* alphaBlendState)
{
    this->device = device;
    this->swapChain = swapChain;
    this->renderTargetView = renderTargetView;
    this->spriteHandler = spriteHandler;
    this->alphaBlendState = alphaBlendState;
}

void Renderer::BeginFrame()
{
    if (device == nullptr || renderTargetView == nullptr || spriteHandler == nullptr)
        return;

    device->ClearRenderTargetView(renderTargetView, clearColor);

    FLOAT blendFactor[4] = { 0, 0, 0, 0 };
    device->OMSetBlendState(alphaBlendState, blendFactor, 0xffffffff);

    spriteHandler->Begin(D3DX10_SPRITE_SORT_TEXTURE);
}

void Renderer::EndFrame()
{
    if (spriteHandler != nullptr)
        spriteHandler->End();

    if (swapChain != nullptr)
        swapChain->Present(0, 0);
}

void Renderer::DrawSprite(Sprite* sprite, float x, float y)
{
    DrawSprite(sprite, x, y, RenderOptions());
}

void Renderer::DrawSprite(Sprite* sprite, float x, float y, const RenderOptions& options)
{
    // TODO: After Sprite has a Draw/BuildDxSprite function, move the old CGame::Draw
    // matrix and DrawSpritesImmediate logic here.
    //
    // Target call chain:
    // GameObject::Render -> Renderer::DrawSprite -> ID3DX10Sprite::DrawSpritesImmediate
    (void)sprite;
    (void)x;
    (void)y;
    (void)options;
}
