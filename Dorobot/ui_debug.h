#pragma once
#include <map>
#include <mutex>

class UI_Debug
{
public:
	UI_Debug(class Dorobot* doroBot);
	~UI_Debug();
	void addDebuginfo(std::string name, float value);
	void addDebuginfo(std::string name, Vec3<float>* value);
	void render();
	void renderAngles();

private:
	std::map<std::string, float> info;
	std::mutex infoMutex;
};

