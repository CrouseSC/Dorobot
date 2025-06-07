#pragma once
#include "pch.h"

class Positioning
{
public:
	Positioning(Dorobot* doroBot);
	bool moveToPosition(const Vec2<float>& position, float errorRoom);
	void turnToPosition(const Vec2<float>& position, float Difference);
	void endMoveForward();
	void startMoveRight(short amount);
	void startMoveForwardHidden(short amount);
	void startMoveForward(short amount);

private:
	Dorobot* doroBot;

};
