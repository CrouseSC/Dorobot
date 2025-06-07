#include "pch.h"
#include "texture_manager.h"

TextureManager::TextureManager(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}

TextureManager::~TextureManager()
{
}

void TextureManager::loadTextureFromFile(const std::string name, const std::string& filePath)
{
	TextureWrapper* texture = new TextureWrapper();
	std::filesystem::path cwd = std::filesystem::current_path();
    bool loaded = LoadTextureFromFile(filePath.c_str(), &texture->id, &texture->size.x, &texture->size.y, doroBot->game->get_device());
	if (loaded) {
		textures.emplace(name, texture);
	}
}

TextureWrapper* TextureManager::getTexture(const std::string& name)
{
	if (textures.contains(name)) {
		return textures[name];
	}
	else {
		return nullptr;
	}
}
