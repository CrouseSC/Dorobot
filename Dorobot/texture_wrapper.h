#pragma once

class TextureWrapper
{
public:
	Vec2<int> size{};
	PDIRECT3DTEXTURE9 id;
	ImTextureID getId() const;
};
