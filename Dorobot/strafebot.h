#pragma once
#include "game_util.h"

class StrafeBot
{
public:
	StrafeBot(class Dorobot* doroBot);
	~StrafeBot();
	void cycle();
	void cycleBeforeAutomatition();
	void cycleBeforeWritePacket();
	void cycleAfterPredictPlayerState();
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
	bool shouldUseStrafeBot();
	void doTilt();
	std::vector<int> getFpsList();
	Dorobot* doroBot;
	float tiltOldPitch;

};
