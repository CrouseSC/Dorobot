#pragma once
#include <chrono>

class SessionManager
{
public:
	SessionManager(Dorobot* dorobot);
	~SessionManager();
	void cycle();
	bool isJH = false;
	bool isVCJ = false;

private:
	Dorobot* doroBot;
	bool connected = false;
	bool sending3XP = false;
};
