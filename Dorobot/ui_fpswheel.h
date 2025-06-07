#pragma once
#include <map>
#include "cod4_structs.h"
#include "vectors.h"
#include "game_math.h"

class UI_FpsWheel
{
public:
	UI_FpsWheel(class Dorobot* doroBot);
	~UI_FpsWheel();
	void render(class Dorobot* doroBot);
private:
	std::multimap<int, Vec2<float>> fpsZones
	{
		//CENTER 90
		{125, Vec2<float>(103.7f, 118.9f)},
		{200, Vec2<float>(94.5f, 103.7f)},
		{250, Vec2<float>(73.6f, 86.9f)},
		{251, Vec2<float>(86.9f, 94.5f)},
		{333, Vec2<float>(28.90f, 73.6f)},
	};;
};
