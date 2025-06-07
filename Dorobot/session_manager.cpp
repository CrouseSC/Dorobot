#include "pch.h"
#include "session_manager.h"

SessionManager::SessionManager(Dorobot* dorobot)
{
	this->doroBot = dorobot;
}

SessionManager::~SessionManager()
{
}

void SessionManager::cycle()
{
	static auto start = std::chrono::steady_clock::now();
	
	bool gameConnected = doroBot->game->isConnected();
	if (gameConnected && !connected) {
		connected = true;
		//doroBot->game->addObituary("CONNECTED");
		std::string modName = doroBot->game->getCvar("fs_game")->value.valueString;
		if (modName.contains("3xp") || modName.contains("3xP") || modName.contains("3XP")) {
			//doroBot->game->addObituary("Conencted to 3xP...");
			sending3XP = true;
			isJH = false;
			isVCJ = false;
			start = std::chrono::steady_clock::now();
			doroBot->game->sendCommandToConsole("openscriptmenu -1 rtu");
		}
		else if (modName.contains("jh") || modName.contains("JH")) {
			isJH = true;
			sending3XP = false;
			isVCJ = false;
			start = std::chrono::steady_clock::now();
		}
		else if (modName.contains("visualcj") || modName.contains("VISUALCJ") || modName.contains("Visualcj")) {
			isJH = false;
			sending3XP = false;
			isVCJ = true;
			start = std::chrono::steady_clock::now();
		}
	}
	else if (!gameConnected && connected) {
		connected = false;
		sending3XP = false;
		//doroBot->game->addObituary("DISCONNECTED");
	}

	if (connected && sending3XP) {
		auto end = std::chrono::steady_clock::now();
		long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed >= 10000) {
			//doroBot->game->addObituary("10s elapsed...");
			doroBot->game->sendCommandToConsole("openscriptmenu -1 tu");
			start = end;
		}
	}
}
