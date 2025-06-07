#pragma once
#include "pch.h"

pmove_t* copyPmove(pmove_t* pmove);  //Deep copy of a pmove_t
void deletePmove(pmove_t* pmove);  //Should be called after copyPmove
void invertCmdSide(usercmd_s* cmd);
bool veloIncreaseInTransferZone(const Vec3<float>& transferZone);
