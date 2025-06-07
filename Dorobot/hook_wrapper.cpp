#include "pch.h"
#include "hook_wrapper.h"

void Hook::replace(int addr, int dest)
{
	if (*(BYTE*)addr == 0xE9 || *(BYTE*)addr == 0xE8)
	{
		destination = (DWORD)dest;
		address = (DWORD)addr;
		trampoline = mem::instructionToAbsoluteAddress(addr);
		DWORD old;
		VirtualProtect((LPVOID)addr, 0x5, PAGE_EXECUTE_READWRITE, &old);
		memcpy(p_orig, (LPVOID)addr, 5);
		*(DWORD*)(addr + 1) = dest - addr - 5;
		VirtualProtect((LPVOID)addr, 0x5, old, NULL);
	}
}

void Hook::detour(int addr, int dest)
{
	//PLH::ZydisDisassembler dis(PLH::Mode::x86);
	phook = new PLH::x86Detour((uint64_t)addr, (uint64_t)dest, (uint64_t*)&trampoline);
	phook->hook();

	mem::copy((int)&local_orig, (BYTE*)dest, 5);
}

bool Hook::isLocalHooked()
{
	BYTE local[5];
	mem::copy((int)&local, (BYTE*)destination, 5);
	return memcmp(local, local_orig, 5) != 0;
}

void Hook::replaceCall(int addr, int dest)
{
	replace(addr, dest);
	mem::copy((int)&local_orig, (BYTE*)dest, 5);
}
void Hook::rehook()
{

	if (hookType == HOOK_TYPE_DETOUR)
	{
		//phook->reHook();
		detour(address, destination);
	}
	else
		replaceCall(address, destination);
}

void Hook::remove()
{
	if (hookType != HOOK_TYPE_DETOUR)
		mem::copy(address, p_orig, 5);
	else
		if (phook)
		{
			phook->unHook();
			delete phook;
		}
}
