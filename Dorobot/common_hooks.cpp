#include "pch.h"
#include "common_hooks.h"

void __fastcall createMove(DWORD eax, DWORD ecx)
{
	if (!Dorobot::getInstance()->game->isConnected() || Dorobot::getInstance()->sessionManager->isJH || Dorobot::getInstance()->sessionManager->isVCJ) {
		Dorobot::getInstance()->hookWrapper->hookMap["CreateMove"]->original(createMove)(eax, ecx);
		return;
	}
	if (Dorobot::getInstance()->game->getPmoveCurrent()->ps) {
		static float prevVelo = Dorobot::getInstance()->game->getPmoveCurrent()->ps->velocity.Length2D();
		float velo = Dorobot::getInstance()->game->getPmoveCurrent()->ps->velocity.Length2D();
		float veloIncrease = (velo - prevVelo) > 0 ? 1 : 0;
		//Dorobot::getInstance()->uiDebug->addDebuginfo("Velocity increase", veloIncrease);
		if (veloIncrease == 0 && velo > 360 && !Dorobot::getInstance()->game->isOnGround()) {
			//Dorobot::getInstance()->game->addObituary("Velocity didn't increase");
		}
		prevVelo = velo;
	}

	Dorobot::getInstance()->recorder->cycle();
	Dorobot::getInstance()->hookWrapper->hookMap["CreateMove"]->original(createMove)(eax, ecx);
}

int writePacket()
{
	return Dorobot::getInstance()->hookWrapper->hookMap["WritePacket"]->original(writePacket)();
}

__declspec(naked)
void renderScene_Stub() noexcept
{
	refdef_t* oRef;
	int tramp;
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE

		mov oRef, eax
	}

	tramp = Dorobot::getInstance()->hookWrapper->hookMap["RenderScene"]->trampoline;
	Dorobot::getInstance()->recorder->cycleEditing(oRef);

	_asm {
		mov eax, oRef
		call tramp
		mov esp, ebp
		pop ebp
		ret
	}
}

int DrawViewhands(int* a, int* b, int* c)
{
	int ret = Dorobot::getInstance()->hookWrapper->hookMap["DrawViewhands"]->original(DrawViewhands)(a, b, c);
	return ret;
}

CommonHooks::CommonHooks(Dorobot* dorobot)
{
	this->dorobot = dorobot;
	dorobot->hookWrapper->Add("CreateMove", addr_createMove, createMove, HOOK_TYPE_DETOUR);
	dorobot->hookWrapper->Add("WritePacket", addr_writePacket, writePacket, HOOK_TYPE_DETOUR);
	dorobot->hookWrapper->Add("RenderScene", addr_renderScene, renderScene_Stub, HOOK_TYPE_DETOUR);
	//dorobot->hookWrapper->Add("DrawViewhands", 0x00646a60, DrawViewhands, HOOK_TYPE_DETOUR);
}

CommonHooks::~CommonHooks()
{
}
