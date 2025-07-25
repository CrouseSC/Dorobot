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
	PredictionValues nextFrameValues;
	PredictionValues calculateBestAngleAndFps(safePmove_t pmove, bool invert = false);
	bool strafeBotCycled;

private:
	PredictionValues calculateBestAngleForGroundStrafe(safePmove_t pmove, bool invert = false);
	PredictionValues calculateBestAngleAndFpsForBothDirections(const safePmove_t& pmove);
	std::unique_ptr<safePmove_t> buildDefaultPmove();
	bool shouldUseStrafeBot();
	std::vector<int> getFpsList();
	Dorobot* doroBot;

};
