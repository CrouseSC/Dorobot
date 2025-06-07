#include "pch.h"
#include "game_util.h"

pmove_t* copyPmove(pmove_t* pmove)  //Deep copy of a pmove_t
{
	pmove_t* newPmove = new pmove_t();
	*newPmove = *pmove;
	playerState_s* newPlayerState = new playerState_s();
	*newPlayerState = *pmove->ps;

	newPmove->ps = newPlayerState;
	return newPmove;
}

void deletePmove(pmove_t* pmove)
{
	delete pmove->ps;
	delete pmove;
}

void invertCmdSide(usercmd_s* cmd)
{
	cmd->side = cmd->side == USERCMD_SIDE_RIGHT ? USERCMD_SIDE_LEFT : USERCMD_SIDE_RIGHT;
}

bool veloIncreaseInTransferZone(const Vec3<float>& transferZone)
{
	Vec2<float> zone2D(transferZone.x, transferZone.y);
	return abs(zone2D.x) == 1 && abs(zone2D.y) == 1;
}
