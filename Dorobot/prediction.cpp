#include "pch.h"
#include "prediction.h"

void callPmoveSingle(pmove_t* pm)
{
    typedef void pMoveSingle(pmove_t*);
    pMoveSingle* func = (pMoveSingle*)addr_pmoveSingle;
    func(pm);
}

void callPmove(pmove_t* pm)
{
	usercmd_s* p_pmcmd;
	int* pmcmd_servertime;
	playerState_s* ps;
	int psCommandTime;
	int serverTime;

	ps = pm->ps;
	serverTime = (pm->cmd).serverTime;
	if (ps->commandTime <= serverTime) {
		if (ps->commandTime + 1000 < serverTime) {
			ps->commandTime = serverTime + -1000;
		}
		*(int*)(&pm->numtouch + 2) = 0;
		psCommandTime = ps->commandTime;
		while (psCommandTime != serverTime) {
			psCommandTime = serverTime - ps->commandTime;
			if (66 < psCommandTime) {
				psCommandTime = 66;
			}
			(pm->cmd).serverTime = ps->commandTime + psCommandTime;
			callPmoveSingle(pm);
			p_pmcmd = &pm->cmd;
			pmcmd_servertime = (int*)((int)&(pm->oldcmd).serverTime + 1);
			memcpy(&pm->oldcmd, &pm->cmd, sizeof(pm->oldcmd));
			psCommandTime = ps->commandTime;
		}
	}

    return;
}

__declspec(naked)
void PM_Accelerate_stub(pml_t* pml, float wishSpeed, float accelType) noexcept  //[IN] wishdir: eax, ps: edi
{
    playerState_s* ps;
    Vec3<float>* wishDir;
    int original;

    _asm {
        push ebp
        mov ebp, esp
        sub esp, __LOCAL_SIZE

        mov ps, edi
        mov wishDir, eax
    }

    original = Dorobot::getInstance()->hookWrapper->hookMap["PM_Accelerate"]->trampoline;

    __asm {        
        push accelType
        push wishSpeed
        push pml
        mov eax, wishDir
        mov edi, ps
        call original
        add esp, 0xC
        mov esp, ebp
        pop ebp
        ret
    }
}

void call_PM_Accelerate(Vec3<float> wishDir, playerState_s* playerState, pml_t* pml, float wishSpeed, float accel)
{
    void* pwishDir = &wishDir.x;

    DWORD dEAX, dEDI;
    __asm {
        mov dEAX, eax
        mov dEDI, edi
        mov eax, pwishDir
        mov edi, playerState
        push accel
        push wishSpeed
        push pml
        call PM_Accelerate_stub
        add esp, 0xC
        mov eax, dEAX
        mov edi, dEDI
    }
}

void callPMAirmove(pmove_t* pm, pml_t* pml)
{
    Dorobot::getInstance()->hookWrapper->hookMap["PM_Airmove"]->original(callPMAirmove)(pm, pml);
}

void Prediction::PM_Accelerate(Vec3<float> wishDir, playerState_s* playerState, pml_t* pml, float wishSpeed, float accel)
{
    call_PM_Accelerate(wishDir, playerState, pml, wishSpeed, accel);
}

void Prediction::PmoveSingle(pmove_t* pm)
{
    callPmoveSingle(pm);
}

pmove_t* Prediction::predictMove(int fps, float angle, bool invert)
{
    pmove_t* pmove = doroBot->game->getPmoveCurrent();
    pmove_t* currentPmove = copyPmove(pmove);

    if (invert)
        currentPmove->cmd.side = currentPmove->cmd.side == USERCMD_SIDE_LEFT ? USERCMD_SIDE_RIGHT : USERCMD_SIDE_LEFT;
    
    currentPmove->cmd.serverTime = currentPmove->oldcmd.serverTime;
    currentPmove->cmd.serverTime += 1000 / fps;
    currentPmove->cmd.angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0, angle, 0)).y);

    doroBot->prediction->PmoveSingle(currentPmove);
    return currentPmove;
}

Prediction::Prediction(Dorobot* dorobot)
{
    this->doroBot = dorobot;
    //dorobot->hookWrapper->Add("Pmove", addr_pmove, callPmove, HOOK_TYPE_DETOUR);
    //dorobot->hookWrapper->Add("PmoveSingle", addr_pmoveSingle, callPmoveSingle, HOOK_TYPE_DETOUR);
    //dorobot->hookWrapper->Add("PM_Accelerate", addr_pm_accelerate, PM_Accelerate_stub, HOOK_TYPE_DETOUR);
    //dorobot->hookWrapper->Add("PM_Airmove", addr_pm_airmove, callPMAirmove, HOOK_TYPE_DETOUR);
    //dorobot->hookWrapper->Add("FUN_0040E860", addr_fun0040e860, callFUN_0040e860, HOOK_TYPE_DETOUR);
}

Prediction::~Prediction()
{
    if (doroBot && doroBot->hookWrapper) {
        if (doroBot->hookWrapper->hookMap.count("Pmove") > 0)
            doroBot->hookWrapper->hookMap["Pmove"]->remove();
        if (doroBot->hookWrapper->hookMap.count("PmoveSingle") > 0)
            doroBot->hookWrapper->hookMap["PmoveSingle"]->remove();
        if (doroBot->hookWrapper->hookMap.count("PM_Accelerate") > 0)
            doroBot->hookWrapper->hookMap["PM_Accelerate"]->remove();
    }
}
