#pragma once
#include "pch.h"

enum Axis { AXIS_X, AXIS_Y };

class Positioning
{
public:
	Positioning(Dorobot* doroBot);
	bool moveToPosition(const Vec2<float>& position, float errorRoom);
	bool moveToElePosition(const Vec2<float>& position, Axis axis);
	void turnToPosition(const Vec2<float>& position, float Difference);
	void endMoveForward();
	void startMoveRight(short amount);
	void startMoveForwardHidden(short amount);
	void startMoveForward(short amount);
	float axisDist(const Vec3<float>& v1, const Vec3<float>& v2, Axis axis);
	float axisDist(const Vec2<float>& v1, const Vec2<float>& v2, Axis axis);

private:
	Dorobot* doroBot;

};
