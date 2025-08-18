#pragma once

class TextureWrapper
{
public:
	Vec2<int> size;
	PDIRECT3DTEXTURE9 id;
	ImTextureID getId() const;
	virtual ~TextureWrapper() {};  //this should be implemented, but currently there is no use case for unloading textures in dorobot, because cod4 clears textures on d3d9 reset
};

class SpritesheetWrapper : public TextureWrapper
{
public:
	Vec2<int> dimensions;
	int frames;
	float delay;

};

struct AnimationFrame
{
	ImTextureID id;
	Vec2<int> size;
	Vec2<float> uv;
};

class Animation
{
public:
	Animation();
	Animation(SpritesheetWrapper* spriteSheet);
	void setSpriteSheet(SpritesheetWrapper* spriteSheet);
	AnimationFrame getCurrentFrame();
	void updateAnimation();

private:
	SpritesheetWrapper* spriteSheet;
	int currentFrame = 0;
	float timeElapsed = 0.f;

};
