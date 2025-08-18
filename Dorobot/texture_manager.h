#pragma once
#include <map>
#include <filesystem>
#include "texture_wrapper.h"
#include "imgui_helper.h"

class TextureManager
{
public:
	TextureManager(Dorobot* doroBot);
	~TextureManager();
	void loadTextureFromFile(const std::string& name, const std::string& filePath);
	void loadSpriteSheet(const std::string& name, const std::string& filePath, int frames, int delay, const Vec2<int>& dimensions);
	TextureWrapper* getTexture(const std::string& name);

private:
	Dorobot* doroBot;
	std::map<std::string, TextureWrapper*> textures;

};
