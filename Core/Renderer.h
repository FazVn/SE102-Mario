class Renderer
{
public:
	void Renderer();

	void Begin();
	void End();
	void Clear();

	void DrawSprite(Sprite* sprite, float x, float y);
};