#pragma once
#include "polyhook2/Detour/ADetour.hpp"
#include "polyhook2/Detour/x86Detour.hpp"
#include "polyhook2/Detour/ADetour.hpp"
#include "polyhook2/Enums.hpp"
#include "memory.h"
#include <unordered_map>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "psapi.lib")

#define czVOID(c) (void)c

enum HOOK_TYPE
{
	HOOK_TYPE_REPLACE_CALL,
	HOOK_TYPE_JMP,
	HOOK_TYPE_DETOUR
};

class Hook
{
private:
	void replaceCall(int addr, int dest);
	void replace(int addr, int dest);
	void detour(int addr, int dest);
	BYTE p_orig[5];
	BYTE m_orig[5];
	PLH::x86Detour* phook;
public: //methods
	BYTE local_orig[5];
	void remove();
	bool isLocalHooked();
	~Hook()
	{
		remove();
	}
	Hook() : address{}, phook{}, p_orig{}, m_orig{}, local_orig{}, destination{}, trampoline{}, hookType{ HOOK_TYPE_DETOUR } { };
	template<typename X, typename T>
	Hook(X addr, T dest, HOOK_TYPE hooktype = HOOK_TYPE_DETOUR, int tramp = -1)
	{

		address = (int)addr;
		destination = (int)dest;
		hookType = hooktype;
		memcpy(m_orig, dest, sizeof(m_orig));
		switch (hookType)
		{
		case HOOK_TYPE_DETOUR:
		{
			detour((int)addr, (int)dest);
			break;
		}
		case HOOK_TYPE_REPLACE_CALL:
		{
			trampoline = tramp;
			replaceCall((int)addr, (int)dest);
			break;
		}
		case HOOK_TYPE_JMP:
		{
			trampoline = tramp;
			replaceCall((int)addr, (int)dest);
			break;
		}
		}
	}
	template<typename T>
	T original(T fnType) {
		czVOID(fnType);
		return (T)trampoline;
	}
	void rehook();
public: //variables
	int destination;
	int address;
	int trampoline;
	HOOK_TYPE hookType;
};
class HookWrapper
{
public:
	std::unordered_map<std::string, Hook*> hookMap;
	template<typename X, typename T>
	Hook* Add(const std::string name, X addr, T fnc, HOOK_TYPE type)
	{
		Hook* x = new Hook(addr, fnc, type);
		if (x)
		{
			hookMap[name] = x;
			x->hookType = type;
		}
		else
		{
			MessageBoxA(nullptr, "Critical error", "Critical error", 0);
		}
		return x;
	}
	HookWrapper() = default;
	~HookWrapper()
	{
		for (auto& hook : hookMap)
		{
			hook.second->remove();
			delete hook.second;
		}
	}
	HookWrapper(const HookWrapper& other) = delete;
	HookWrapper& operator=(const HookWrapper& other) = delete;
};

