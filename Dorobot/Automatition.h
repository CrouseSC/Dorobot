#pragma once

class Automatition
{
public:
	Automatition(Dorobot* doroBot);
	~Automatition();
	void cycle();
	void cycleAfterStrafebot();
	void registerBinds();

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
	Dorobot* doroBot;

	bool doingAutopara;
	bool hasDoneAutoPara;
	int autoParaDirection;
	bool doingTransferzoneSpam;
	bool hasDoneTransferzoneSpam;
	int transferzoneSpamDirection;
	bool doingAuto250;
	bool hasDoneAuto250;
	int Auto250Direction;
	bool inTransferZoneLast = false;
	bool isJumping;  //set true when a player jumps, reset when he reaches 0 velocity

};
