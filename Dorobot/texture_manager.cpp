#include "pch.h"
#include "texture_manager.h"

TextureManager::TextureManager(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}

TextureManager::~TextureManager()
{
}

void TextureManager::loadTextureFromFile(const std::string& name, const std::string& filePath)
{
	TextureWrapper* texture = new TextureWrapper();
	std::filesystem::path cwd = std::filesystem::current_path();
	std::string path = cwd.string() + "/Dorobot/Assets" + filePath;
    bool loaded = LoadTextureFromFile(path.c_str(), &texture->id, &texture->size.x, &texture->size.y, doroBot->game->get_device());

	if (loaded) {
		textures.emplace(name, texture);
	}
}

void TextureManager::loadSpriteSheet(const std::string& name, const std::string& filePath, int frames, int delay, const Vec2<int>& dimensions)
{
	SpritesheetWrapper* spriteSheet = new SpritesheetWrapper();
	std::filesystem::path cwd = std::filesystem::current_path();
	std::string path = cwd.string() + "/Dorobot/Assets/" + filePath;
	bool loaded = LoadTextureFromFile(path.c_str(), &spriteSheet->id, &spriteSheet->size.x, &spriteSheet->size.y, doroBot->game->get_device());

	if (loaded) {
		spriteSheet->frames = frames;  //LOAD FROM JSON
		spriteSheet->delay = delay;
		spriteSheet->dimensions = dimensions;
		textures.emplace(name, spriteSheet);
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
