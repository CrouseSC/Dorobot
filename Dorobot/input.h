#pragma once
#include <Windows.h>
#include <functional>
#include <vector>

typedef std::function<bool(UINT state)> InputCallback;
class Input
{
public:
	Input(class Dorobot* doroBot);
	~Input();
	bool handleKey(UINT keyCode, UINT state);
	WNDPROC pWndproc = nullptr;
	HWND windowHandle;
	void updateWndproc(HWND handle);
	void addCallback(UINT key, InputCallback fn);
private:
	std::vector<std::pair<UINT,InputCallback>> callbacksInput{};
};

