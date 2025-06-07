#include "pch.h"
#include "Input.h"
#include "Dorobot.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND WINAPI get_foreground_window()
{
	Dorobot* doroBot = Dorobot::getInstance();
	HWND orig = doroBot->hookWrapper->hookMap["GetForegroundWindow"]->original(get_foreground_window)();
	if (doroBot->wantInput)
		return 0; //tell the game that it isn't the foreground window
	return orig;
}

Input::Input(Dorobot* doroBot)
{
	windowHandle = nullptr;
	doroBot->hookWrapper->Add("GetForegroundWindow", GetProcAddress(GetModuleHandleA("user32.dll"), "GetForegroundWindow"), get_foreground_window, HOOK_TYPE_DETOUR);
}

Input::~Input()
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->hookWrapper)
	{
		doroBot->hookWrapper->hookMap["GetForegroundWindow"]->remove(); //remove hook here in case of a race condition on destructors
	}
	SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)pWndproc);
}

bool Input::handleKey(UINT key_code, UINT state)
{

	for (auto &[ key, fn ] : callbacksInput)
	{
		if (key == key_code && fn(state)) //if the callback returns true then return true here as well so we can block the input
			return true;
	}
	if (key_code == VK_ESCAPE && Dorobot::getInstance()->wantInput)
	{
		Dorobot::getInstance()->wantInput = false;
		return true;
	}
	return false;
}

LRESULT __stdcall wndproc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!Dorobot::getInstance())
		return 1;
	
	if (Dorobot::getInstance()->wantInput) //only give input information to imgui if we want input so it doesn't get clicked and dragged or typed in while just playing
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);


	if (Dorobot::getInstance()->game->isFocused())
	{
		if (uMsg == WM_KEYUP || uMsg == WM_KEYDOWN)
		{
			
			if (Dorobot::getInstance()->input->handleKey(wParam, uMsg))
				return 1; //the key was handled and blocked by the handle key function
		}
	}

	if (Dorobot::getInstance()->wantInput)
		return 1;

	return CallWindowProc(Dorobot::getInstance()->input->pWndproc, hWnd, uMsg, wParam, lParam);
}
void Input::addCallback(UINT key, InputCallback fn)
{
	callbacksInput.push_back({ key, fn });
}
void Input::updateWndproc(HWND handle)
{
	if (windowHandle != handle)
	{
		if (pWndproc)
			SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)pWndproc);
		windowHandle = handle;
		pWndproc = (WNDPROC)(SetWindowLongPtr(windowHandle, GWLP_WNDPROC, (LONG_PTR)wndproc_hook));
	}
}