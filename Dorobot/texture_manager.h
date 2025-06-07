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
	void loadTextureFromFile(const std::string name, const std::string& filePath);
	TextureWrapper* getTexture(const std::string& name);

private:
	Dorobot* doroBot;
	std::map<std::string, TextureWrapper*> textures;

};
