#pragma once
#include "game_util.h"

class StrafeBot
{
public:
	StrafeBot(class Dorobot* doroBot);
	~StrafeBot();
	void cycle();
	void invertStrafeAfterCycle();
	int lastFps;
	void setGameToBotValues();
	void registerBinds();
	int bestFps;
	int bestFpsInvert;
	bool nextFrameShotRpg;
	Vec3<float> predictedOptimalView;
	Vec3<float> predictedOptimalViewInvert;
	Vec3<float> predictedVeloIncreaseVec;
	Vec3<float> predictedVeloIncreaseVecInvert;
	float predictedVeloIncrease;
	float predictedVeloIncreaseInvert;

private:
	void calculateBestAngleAndFps(bool invert = false);
	bool shouldUseStrafeBot();
	std::vector<int> getFpsList();
	Dorobot* doroBot;

};
