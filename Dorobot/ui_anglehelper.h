#pragma once
#include "Dorobot.h"
#include "imgui.h"
#include "vectors.h"
#include "game_math.h"

class UI_Anglehelper
{
public:
	UI_Anglehelper(class Dorobot* doroBot);
	~UI_Anglehelper();
	void render(Dorobot*& doroBot, ImVec4& color);
	void renderOnWheel(Dorobot*& doroBot, ImVec4& color);
};

