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

void Prediction::predictMove(int fps, float angle, bool invert, safePmove_t& pmove)
{
    input_s* input = (input_s*)0xCC4FF8;
    usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

    //currentPmove->cmd.side = cmd->side;  //in case automatition changes cmd->side (keyonw, for example)
    pmove.pm->cmd = *cmd;  //in case automatition changes cmd->side (keyonw, for example)

    if (invert)
        pmove.pm->cmd.side = pmove.pm->cmd.side == USERCMD_SIDE_LEFT ? USERCMD_SIDE_RIGHT : USERCMD_SIDE_LEFT;
    
    //pmove.pm->cmd.serverTime = pmove.pm->oldcmd.serverTime;
    pmove.pm->cmd.serverTime = pmove.pm->ps->commandTime;
    pmove.pm->cmd.serverTime += 1000 / fps;
    pmove.pm->cmd.angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0, angle, 0)).y);

    if (pmove.pm->cmd.side == 84) {  //what the fuck?
        pmove.pm->cmd.side = 127;
    }
    if (pmove.pm->cmd.side == -84) {
        pmove.pm->cmd.side = -127;
    }

    doroBot->prediction->PmoveSingle(pmove.pm);
}

std::vector<playerState_s*> Prediction::predictFramesAhead()
{
    auto playerStates = std::vector<playerState_s*>();
    return playerStates;
}

__declspec(naked)
void clientThink_real(gentity_s* ent) noexcept  //usercmd_s* in: EAX
{
    usercmd_s* cmd;
    int original;

    _asm {
        push ebp
        mov ebp, esp
        sub esp, __LOCAL_SIZE

        mov cmd, eax
    }

    original = Dorobot::getInstance()->hookWrapper->hookMap["clientThink_real"]->trampoline;

    _asm {
        mov eax, cmd
        push ent
        call original
    }

    Dorobot::getInstance()->prediction->realPlayerState = ent->client->ps;
    Dorobot::getInstance()->uiDebug->addDebuginfo("clientThink_real DeltaY", ent->client->ps.DeltaAngles.y);

    _asm {
        mov esp, ebp
        pop ebp
        ret
    }
}

void predictPlayerState(int unk)
{
    playerState_s* ps = (playerState_s*)(addr_playerState);
    Dorobot::getInstance()->uiDebug->addDebuginfo("predictPlayerState DeltaY", ps->DeltaAngles.y);

    if ((!Dorobot::getInstance()->game->isConnected() || Dorobot::getInstance()->sessionManager->isJH || Dorobot::getInstance()->sessionManager->isVCJ)
        || !Dorobot::getInstance()->game->getPmoveCurrent()->ps) {
        Dorobot::getInstance()->hookWrapper->hookMap["cg_predictPlayerState"]->original(predictPlayerState)(unk);
        return;
    }

	input_s* input = (input_s*)0xCC4FF8;
	usercmd_s* cmd1 = input->GetUserCmd(input->currentCmdNum);
	usercmd_s* cmd2 = input->GetUserCmd(input->currentCmdNum - 1);

	Dorobot::getInstance()->automatition->cycle();
	Dorobot::getInstance()->strafeBot->cycle();
	Dorobot::getInstance()->automatition->cycleAfterStrafebot();

	if (Dorobot::getInstance()->uiMenu->pfps_toggle && Dorobot::getInstance()->game->getVelocity().Length2D() > 0 && !Dorobot::getInstance()->recorder->isPlayingRecording
		&& !Dorobot::getInstance()->uiMenu->isEditing) {
		int fps = Dorobot::getInstance()->game->get_fps();

		cmd1->serverTime = cmd2->serverTime;
		cmd1->serverTime += 1000 / fps;
	}
    Dorobot::getInstance()->recorder->cycleEditingAfterCreatemove();
    Dorobot::getInstance()->hookWrapper->hookMap["cg_predictPlayerState"]->original(predictPlayerState)(unk);
}


snapshot_s* cg_readNextSnapshot()
{
    snapshot_s* snap = Dorobot::getInstance()->hookWrapper->hookMap["cg_readNextSnapshot"]->original(cg_readNextSnapshot)();
    if (Dorobot::getInstance()->game->isDevmap() && snap) {  //we can sync the client with the server on devmap for perfect prediction
        snap->ps.DeltaAngles = Dorobot::getInstance()->prediction->realPlayerState.DeltaAngles;
    }

    return snap;
}

Prediction::Prediction(Dorobot* dorobot)
{
    this->doroBot = dorobot;
    dorobot->hookWrapper->Add("cg_predictPlayerState", addr_cg_predictPlayerState, predictPlayerState, HOOK_TYPE_DETOUR);
    dorobot->hookWrapper->Add("clientThink_real", addr_clientThink_real, clientThink_real, HOOK_TYPE_DETOUR);
    dorobot->hookWrapper->Add("cg_readNextSnapshot", addr_cg_readNextSnapshot, cg_readNextSnapshot, HOOK_TYPE_DETOUR);
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
