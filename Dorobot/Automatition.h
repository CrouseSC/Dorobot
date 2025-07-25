#pragma once

enum class Direction { NONE = 0, LEFT = USERCMD_SIDE_LEFT, RIGHT = USERCMD_SIDE_RIGHT};
class Automatition
{
public:
	Automatition(Dorobot* doroBot);
	~Automatition();
	void cycle();
	void cycleAfterStrafebot();
	void registerBinds();
	bool isJumping;  //set true when a player jumps, reset when he reaches 0 velocity
	Direction forcedDirection = Direction::NONE;
	Direction bounceSwitchDirection = Direction::NONE;

private:
	void keyOnW();
	void sprintToPrestrafe(bool wd);
	void jumpAtVelo(float velo);
	void rpgLookdown();
	void rpgJump();
	void autopara();
	void autoTransferzone3Spam();
	void auto2Spam();
	void bhop();
	void bhopAfterCycle();
	void yawScript();
	void switchOnBounce();
	Dorobot* doroBot;

	bool doingAutopara;
	bool hasDoneAutoPara;
	bool doingTransferzoneSpam;
	bool hasDoneTransferzoneSpam;
	bool doingAuto250;
	bool hasDoneAuto250;
	bool inTransferZoneLast = false;
	bool switchedOnBounce = false;

};
