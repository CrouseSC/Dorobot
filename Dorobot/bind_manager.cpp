#include "pch.h"
#include "bind_manager.h"


int Bind::idCounter = 0;

BindManager::BindManager(Dorobot* doroBot)
{
	this->doroBot = doroBot;
	setupAllowedKeys();
}

BindManager::~BindManager()
{
}

void BindManager::cycle()
{
	std::unordered_set<int> pressedKeys;

	for (int keyCode : allowedKeys) {
		if (GetAsyncKeyState(keyCode) & 0x8000) {
			pressedKeys.insert(keyCode);
		}
	}

	for (auto& bind : binds) {
		if (bind.type == BIND_TYPE_HOLD) {
			bind.active = pressedKeys.contains(bind.key);
		}
		else if(bind.type == BIND_TYPE_TOGGLE) {
			if (pressedKeys.contains(bind.key) && !previousPressedKeys.contains(bind.key)) {
				bind.active = !bind.active;
			}
		}
	}

	previousPressedKeys = pressedKeys;
}

void BindManager::addBind(const std::string& name, int key)
{
	binds.push_back(Bind(name, key, BIND_TYPE_NONE));
}

void BindManager::deleteBind(const Bind& bind)
{
	for (auto it = binds.begin(); it != binds.end(); ++it) {
		if (it->id == bind.id) {
			binds.erase(it);
			return;
		}
	}
}

void BindManager::registerBindName(const std::string& name, BIND_TYPE type)
{
	possibleBinds.emplace(name, type);
}

bool BindManager::bindActive(const std::string& name)
{
	bool active = false;
	for (auto& bind : binds) {
		if (bind.name == name) {
			active = active || bind.active;
		}
	}

	return active;
}

std::vector<std::string> BindManager::getPossibleBindNames() const
{
	std::vector<std::string> names;
	for (const auto& [key, _] : possibleBinds) {
		names.push_back(key);
	}

	return names;
}

void BindManager::setBindName(Bind& bind, const std::string& name)
{
	bind.name = name;
	bind.type = possibleBinds[name];
}

int BindManager::getCurrentPressedKey() const
{
	if (previousPressedKeys.empty()) {
		return -1; //no keys pressed
	}
	return *previousPressedKeys.begin(); //good enough.
}

std::string BindManager::getNameForKey(int key) const
{
	if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9')) {
		return std::string(1, (char)key);
	}
	else {
		switch (key) {
			case VK_RBUTTON: return "Right Mouse Button";
			case VK_MBUTTON: return "Middle Mouse Button";
			case VK_TAB: return "Tab";
			case VK_RETURN: return "Enter";
			case VK_MENU: return "Alt";
			case VK_LSHIFT: return "Left Shift";
			case VK_RSHIFT: return "Right Shift";
			case VK_CAPITAL: return "Caps Lock";
			case VK_CONTROL: return "Control";
			case 0x70: return "F1";
			case 0x71: return "F2";
			case 0x72: return "F3";
			case 0x73: return "F4";
			case 0x74: return "F5";
			case 0x75: return "F6";
			case 0x76: return "F7";
			case 0x77: return "F8";
			case 0x78: return "F9";
			case 0x79: return "F10";
			case 0x7A: return "F11";
			case 0x7B: return "F12";
			case VK_SPACE: return "Space";
			case VK_XBUTTON1: return "Mouse4";
			case VK_XBUTTON2: return "Mouse5";
			case -1: return "...";
			default: return "Unknown Key (" + std::to_string(key) + ")";
		}
	}
}

void BindManager::setupAllowedKeys()
{
	for (char c = 'A'; c <= 'Z'; c++) {
		allowedKeys.push_back(c);
	}
	for (char c = '0'; c <= '9'; c++) {
		allowedKeys.push_back(c);
	}

	allowedKeys.push_back(VK_RBUTTON);
	allowedKeys.push_back(VK_MBUTTON);
	allowedKeys.push_back(VK_TAB);
	allowedKeys.push_back(VK_RETURN);
	allowedKeys.push_back(VK_MENU); //alt
	allowedKeys.push_back(VK_LSHIFT);
	allowedKeys.push_back(VK_RSHIFT);
	allowedKeys.push_back(VK_CAPITAL); //caps lock
	allowedKeys.push_back(VK_CONTROL);
	allowedKeys.push_back(VK_SPACE);
	allowedKeys.push_back(VK_XBUTTON1);
	allowedKeys.push_back(VK_XBUTTON2);
	for (int i = 0x70; i <= 0x7B; i++) { //F1-F12
		allowedKeys.push_back(i);
	}
}
