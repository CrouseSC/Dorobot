#include "pch.h"
#include "Automatition.h"

Automatition::Automatition(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}


Automatition::~Automatition()
{

}

void Automatition::registerBinds()
{
	doroBot->bindManager->registerBindName("Autopara", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Auto 232 in transferzone", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Auto 250", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Bhop", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Yaw script", BIND_TYPE_HOLD);
}

void Automatition::keyOnW()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doroBot->uiMenu->strafebot_toggle  && !Dorobot::getInstance()->game->isOnGround()) {
		Lmove lmove = Dorobot::getInstance()->game->getLmove();
		if (lmove.isForward && !lmove.isRight && !lmove.isLeft) {
			cmd->side = doroBot->uiMenu->keyOnWWDSelected ? USERCMD_SIDE_RIGHT : USERCMD_SIDE_LEFT;
		}
	}
}

void Automatition::sprintToPrestrafe(bool wd)
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (cmd->buttons & USERCMD_BUTTONS_SPRINT) {
		cmd->forward |= USERCMD_FORWARD_FORWARD;
		cmd->side |= (wd ? USERCMD_SIDE_RIGHT : USERCMD_SIDE_LEFT);
	}
}

void Automatition::jumpAtVelo(float veloToJumpAt)
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	float vel = doroBot->game->getVelocity().Length2D();
	float vel3D = doroBot->game->getVelocity().Length3D();
	if (!doroBot->game->isOnGround()) {
		isJumping = true;
	}
	if (vel >= veloToJumpAt && !isJumping) {
		cmd->buttons |= USERCMD_BUTTONS_JUMP;
		isJumping = true;
	}

	if (isJumping && vel3D == 0.f) {
		isJumping = false;
	}
}

void Automatition::rpgLookdown()
{
	playerState_s* ps = doroBot->game->getPmoveCurrent()->ps;
	if (!doroBot->uiMenu->rpgLookdown_toggle || !ps) {
		return;
	}

	static bool shot = false;
	static bool resetPitch = false;
	static float prevPitch = doroBot->game->getView().x;
	static float prevYaw = doroBot->game->getView().y;

	if (resetPitch) {
		doroBot->game->setPitch(prevPitch);
		if (doroBot->uiMenu->rpgLookdownBackwards) {
			doroBot->game->setYaw(prevYaw);
		}
		resetPitch = false;
	}

	bool holdingRpg = ps->weapon == doroBot->game->callBG_FindWeaponIndexForName("rpg_mp")
		|| ps->weapon == doroBot->game->callBG_FindWeaponIndexForName("rpg_sustain_mp");

	if (doroBot->strafeBot->nextFrameShotRpg && holdingRpg && ps->weaponState != 7) {
		prevPitch = doroBot->game->getView().x;
		prevYaw = doroBot->game->getView().y;
		input_s* input = doroBot->game->getInput_s();
		usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
		doroBot->game->setPitch(doroBot->uiMenu->rpgLookdownPitch);
		if (doroBot->uiMenu->rpgLookdownBackwards) {
			doroBot->game->setYaw(doroBot->uiMenu->rpgLookdownYaw);
			cmd->angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0.f, doroBot->uiMenu->rpgLookdownYaw, 0.f)).y);
		}
		cmd->angles[0] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(doroBot->uiMenu->rpgLookdownPitch, 0.f, 0.f)).x);
		resetPitch = true;
	}

	shot = doroBot->strafeBot->nextFrameShotRpg;
}

void Automatition::rpgJump()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doroBot->uiMenu->jumpOnRpg_toggle && doroBot->strafeBot->nextFrameShotRpg) {
		cmd->buttons |= USERCMD_BUTTONS_JUMP;
	}
}

void Automatition::autopara()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	if (doroBot->game->isOnGround() || !doroBot->uiMenu->autopara_toggle || doroBot->game->getVelocity().Length2D() > 1500.f) {
		doingAutopara = false;
		hasDoneAutoPara = false;
		return;
	}
	if (doroBot->strafeBot->bestFps == 333) {
		return;
	}
	if (hasDoneAutoPara && doroBot->uiMenu->autoparaOnlyOnce) {
		return;
	}

	if (doroBot->bindManager->bindActive("Autopara")) {
		if (doingAutopara) {
			if (doroBot->strafeBot->lastFps == 333 && doroBot->strafeBot->bestFpsInvert == 333) {
				invertCmdSide(cmd);
				autoParaDirection = cmd->side;
				doroBot->strafeBot->invertStrafeAfterCycle();
			}
			else if (doroBot->strafeBot->bestFpsInvert != 333) {
				doingAutopara = false;
				hasDoneAutoPara = true;
			}
		}
		else if (doroBot->strafeBot->lastFps == 333 && doroBot->strafeBot->bestFpsInvert == 333) {
			invertCmdSide(cmd);
			autoParaDirection = cmd->side;
			doroBot->strafeBot->invertStrafeAfterCycle();
			doingAutopara = true;
		}
	}
	else {
		doingAutopara = false;
	}
}

void Automatition::autoTransferzone3Spam()
{
	bool inTransferZone = veloIncreaseInTransferZone(doroBot->strafeBot->predictedVeloIncreaseVec) && doroBot->strafeBot->bestFps == 250;
	bool inTransferZoneInvert = veloIncreaseInTransferZone(doroBot->strafeBot->predictedVeloIncreaseVecInvert) && doroBot->strafeBot->bestFpsInvert == 250;

	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	if (doroBot->game->isOnGround() || !doroBot->uiMenu->autoTransferzone3Spam_toggle) {
		doingTransferzoneSpam = false;
		hasDoneTransferzoneSpam = false;
		inTransferZoneLast = inTransferZone;
		return;
	}
	else if (!inTransferZone && !inTransferZoneInvert && !inTransferZoneLast) {
		doingTransferzoneSpam = false;
		hasDoneTransferzoneSpam = false;
		inTransferZoneLast = inTransferZone;
		return;
	}
	if (hasDoneTransferzoneSpam && doroBot->uiMenu->autoTransferzone3SpamOnlyOnce) {
		inTransferZoneLast = inTransferZone;
		return;
	}

	bool shouldSwitch = (inTransferZoneLast && !inTransferZone && doroBot->strafeBot->bestFpsInvert == 333) || (inTransferZoneInvert && doroBot->strafeBot->bestFps != 333);

	//doroBot->uiDebug->addDebuginfo("shouldSwitch", shouldSwitch);
	if (doroBot->bindManager->bindActive("Auto 232 in transferzone")) {
		if (doingTransferzoneSpam) {
			if (shouldSwitch) {
				invertCmdSide(cmd);
				transferzoneSpamDirection = cmd->side;
				doroBot->strafeBot->invertStrafeAfterCycle();
			}
		}
		else if (shouldSwitch) {
			invertCmdSide(cmd);
			transferzoneSpamDirection = cmd->side;
			doroBot->strafeBot->invertStrafeAfterCycle();
			doingTransferzoneSpam = true;
		}
	}
	else {
		doingTransferzoneSpam = false;
	}
	inTransferZoneLast = inTransferZone;
}

void Automatition::auto2Spam()
{
	bool inTransferZone = veloIncreaseInTransferZone(doroBot->strafeBot->predictedVeloIncreaseVec) && doroBot->strafeBot->bestFps == 250;
	bool inTransferZoneInvert = veloIncreaseInTransferZone(doroBot->strafeBot->predictedVeloIncreaseVecInvert) && doroBot->strafeBot->bestFpsInvert == 250;
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	if (doroBot->game->isOnGround() || !doroBot->uiMenu->auto250_toggle) {
		doingAuto250 = false;
		hasDoneAuto250 = false;
		return;
	}
	if (doroBot->strafeBot->bestFps != 250) {
		return;
	}
	if (hasDoneAuto250) {
		return;
	}

	if (doroBot->bindManager->bindActive("Auto 250")) {
		if (doingAuto250) {
			if (inTransferZone && !inTransferZoneInvert && doroBot->strafeBot->predictedVeloIncreaseInvert > 0) {
				invertCmdSide(cmd);
				Auto250Direction = cmd->side;
				doroBot->strafeBot->invertStrafeAfterCycle();
			}
			else if (doroBot->strafeBot->bestFpsInvert != 250) {
				doingAuto250 = false;
				hasDoneAuto250 = true;
			}
		}
		else if (inTransferZone && !inTransferZoneInvert && doroBot->strafeBot->predictedVeloIncreaseInvert > 0) {
			invertCmdSide(cmd);
			Auto250Direction = cmd->side;
			doroBot->strafeBot->invertStrafeAfterCycle();
			doingAuto250 = true;
		}
	}
	else {
		doingAuto250 = false;
	}
}

void Automatition::bhop()
{
	static bool wasOnGround = false;

	if (doroBot->uiMenu->bhop_toggle && doroBot->bindManager->bindActive("Bhop")) {
		input_s* input = doroBot->game->getInput_s();
		usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
		if (doroBot->strafeBot->nextFrameOnGround) {  //if were going to be touching the ground on the next frame, release the jump button
			cmd->buttons |= USERCMD_BUTTONS_JUMP;
		}
	}

	wasOnGround = doroBot->game->isOnGround();
}

void Automatition::bhopAfterCycle()
{
	bool isInAir = Dorobot::getInstance()->game->getPmoveCurrent()->ps->GroundEntityNum == 1023;
	if (isInAir && doroBot->strafeBot->nextFrameOnGround) {
		input_s* input = doroBot->game->getInput_s();
		usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

		cmd->buttons &= !USERCMD_BUTTONS_JUMP;
	}
}

void Automatition::yawScript()
{
	if (doroBot->uiMenu->yawScript_toggle && doroBot->bindManager->bindActive("Yaw script")) {
		input_s* input = doroBot->game->getInput_s();
		usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
		cmd->angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0.f, doroBot->uiMenu->yawScriptAngle, 0.f)).y);
		doroBot->game->setYaw(doroBot->uiMenu->yawScriptAngle);
	}
}

void Automatition::cycleAfterStrafebot()
{
	autopara();
	autoTransferzone3Spam();
	auto2Spam();
	rpgLookdown();
	rpgJump();
	bhopAfterCycle();
	yawScript();
}

void Automatition::cycle()
{
	bhop();

	if (doroBot->game->isSpectating() || doroBot->game->isNocliping()) {
		return;
	}

	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doroBot->uiMenu->strafebot_toggle && !doroBot->game->isOnGround() && doroBot->uiMenu->keyOnW_toggle) {
		keyOnW();
	}

	if (doroBot->uiMenu->strafebot_toggle && doroBot->uiMenu->sprintToPrestrafe_toggle && doroBot->game->isOnGround()) {
		sprintToPrestrafe(doroBot->uiMenu->sprintToPrestrafeIsWD);
	}

	if (doroBot->uiMenu->jumpAtVelo_toggle) {
		jumpAtVelo(doroBot->uiMenu->veloToJumpAt);
	}

	if (doingAutopara) {
		cmd->side = autoParaDirection;
	}
	if (doingTransferzoneSpam) {
		cmd->side = transferzoneSpamDirection;
	}
	if (doingAuto250) {
		cmd->side = Auto250Direction;
	}
}
