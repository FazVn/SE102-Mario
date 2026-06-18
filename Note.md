# Render callchain cũ: 

main.cpp
→ Render()
→ CGame::Render() hoặc currentScene->Render()
→ CPlayScene::Render()
→ CGameObject::Render() của từng object
→ CMario::Render() / CGoomba::Render() / CCoin::Render()
→ CAnimations::Get(id)->Render(x, y)
→ CAnimation::Render(x, y)
→ CAnimationFrame::GetSprite()
→ CSprite::Draw(x, y)
→ CGame::GetSpriteHandler()->DrawSpritesImmediate()
→ DirectX vẽ lên màn hình