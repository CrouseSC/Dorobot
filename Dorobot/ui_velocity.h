#pragma once
#include "imgui.h"
#include "vectors.h"

class UI_Velocity
{
public:
	UI_Velocity(class Dorobot* doroBot);
	~UI_Velocity();
	void render(Dorobot*& doroBot, bool& is_locked, Vec2<float>& pos, float& scale, ImVec4& color);
};

