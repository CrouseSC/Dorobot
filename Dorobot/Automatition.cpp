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
	doroBot->bindManager->registerBindName("Strafe switch on bounce", BIND_TYPE_HOLD);
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
	auto ps = doroBot->game->getPmoveCurrent()->ps;

	if (ps) {
		float vel = ps->velocity.Length2D();
		float vel3D = ps->velocity.Length3D();
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

	if (doroBot->strafeBot->nextFrameValues.shotRpg && holdingRpg && ps->weaponState != 7) {
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

	shot = doroBot->strafeBot->nextFrameValues.shotRpg;
}

void Automatition::rpgJump()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doroBot->uiMenu->jumpOnRpg_toggle && doroBot->strafeBot->nextFrameValues.shotRpg) {
		cmd->buttons |= USERCMD_BUTTONS_JUMP;
	}
}

void Automatition::autopara()
{
	if (doingAuto250 || doingTransferzoneSpam) {
		return;
	}
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	if (doroBot->game->isOnGround() || !doroBot->uiMenu->autopara_toggle || doroBot->game->getVelocity().Length2D() > 1500.f) {
		doingAutopara = false;
		hasDoneAutoPara = false;
		forcedDirection = Direction::NONE;
		return;
	}
	if (doroBot->strafeBot->nextFrameValues.fps == 333) {
		return;
	}
	if (hasDoneAutoPara && doroBot->uiMenu->autoparaOnlyOnce) {
		return;
	}

	if (doroBot->bindManager->bindActive("Autopara")) {
		if (doingAutopara) {
			if (doroBot->strafeBot->lastFps == 333 && doroBot->strafeBot->nextFrameValues.fpsInvert == 333) {
				invertCmdSide(cmd);
				forcedDirection = static_cast<Direction>(cmd->side);
				doroBot->strafeBot->invertStrafeAfterCycle();
			}
			else if (doroBot->strafeBot->nextFrameValues.fpsInvert != 333) {
				doingAutopara = false;
				hasDoneAutoPara = true;
				forcedDirection = Direction::NONE;
			}
		}
		else if (doroBot->strafeBot->lastFps == 333 && doroBot->strafeBot->nextFrameValues.fpsInvert == 333) {
			invertCmdSide(cmd);
			forcedDirection = static_cast<Direction>(cmd->side);
			doroBot->strafeBot->invertStrafeAfterCycle();
			doingAutopara = true;
		}
	}
	else {
		doingAutopara = false;
		forcedDirection = Direction::NONE;
	}
}

void Automatition::autoTransferzone3Spam()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doingAutopara || doingAuto250) {
		return;
	}

	if (doroBot->game->isOnGround()) {
		doingTransferzoneSpam = false;
		forcedDirection = Direction::NONE;
	}

	bool inTransferZone = veloIncreaseInTransferZone(doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseVec) && doroBot->strafeBot->nextFrameValues.fps == 250;
	bool inTransferZoneInvert = veloIncreaseInTransferZone(doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseVecInvert) && doroBot->strafeBot->nextFrameValues.fpsInvert == 250;

	if (doroBot->bindManager->bindActive("Auto 232 in transferzone")) {
		if (inTransferZoneLast && !inTransferZone && doroBot->strafeBot->nextFrameValues.fpsInvert == 333) {
			doingTransferzoneSpam = true;
			invertCmdSide(cmd);
			forcedDirection = static_cast<Direction>(cmd->side);
			doroBot->strafeBot->invertStrafeAfterCycle();
			//doroBot->game->addObituary("SWITCH 1");
		}
		else if (doroBot->strafeBot->lastFps == 333 && doroBot->strafeBot->nextFrameValues.fps != 333 && inTransferZoneInvert && !inTransferZone) {
			doingTransferzoneSpam = true;
			invertCmdSide(cmd);
			forcedDirection = static_cast<Direction>(cmd->side);
			doroBot->strafeBot->invertStrafeAfterCycle();
			//doroBot->game->addObituary("SWITCH 2");
		}
		else if (doroBot->strafeBot->nextFrameValues.fps != 333 && !inTransferZoneInvert && !inTransferZone) {
			forcedDirection = Direction::NONE;
			doingTransferzoneSpam = false;
		}
	}
	else {
		forcedDirection = Direction::NONE;
		doingTransferzoneSpam = false;
	}

	inTransferZoneLast = inTransferZone;
}

void Automatition::auto2Spam()
{
	if (doingAutopara || doingTransferzoneSpam) {
		return;
	}

	bool inTransferZone = veloIncreaseInTransferZone(doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseVec) && doroBot->strafeBot->nextFrameValues.fps == 250;
	bool inTransferZoneInvert = veloIncreaseInTransferZone(doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseVecInvert) && doroBot->strafeBot->nextFrameValues.fpsInvert == 250;
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	if (doroBot->game->isOnGround() || !doroBot->uiMenu->auto250_toggle) {
		doingAuto250 = false;
		hasDoneAuto250 = false;
		forcedDirection = Direction::NONE;
		return;
	}
	if (doroBot->strafeBot->nextFrameValues.fps != 250) {
		return;
	}
	if (hasDoneAuto250) {
		return;
	}

	if (doroBot->bindManager->bindActive("Auto 250")) {
		if (doingAuto250) {
			if (inTransferZone && !inTransferZoneInvert && doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseInvert > 0) {
				invertCmdSide(cmd);
				forcedDirection = static_cast<Direction>(cmd->side);
				doroBot->strafeBot->invertStrafeAfterCycle();
			}
			else if (doroBot->strafeBot->nextFrameValues.fpsInvert != 250) {
				doingAuto250 = false;
				hasDoneAuto250 = true;
				forcedDirection = Direction::NONE;
			}
		}
		else if (inTransferZone && !inTransferZoneInvert && doroBot->strafeBot->nextFrameValues.predictedVeloIncreaseInvert > 0) {
			invertCmdSide(cmd);
			forcedDirection = static_cast<Direction>(cmd->side);
			doroBot->strafeBot->invertStrafeAfterCycle();
			doingAuto250 = true;
		}
	}
	else {
		doingAuto250 = false;
		forcedDirection = Direction::NONE;
	}
}

void Automatition::bhop()
{
	if (doroBot->game->isFocused()) {
		if (doroBot->uiMenu->bhop_toggle && doroBot->bindManager->bindActive("Bhop")) {
			input_s* input = doroBot->game->getInput_s();
			usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
			if (doroBot->strafeBot->nextFrameValues.onGround) {
				cmd->buttons |= USERCMD_BUTTONS_JUMP;
			}
		}
	}
}

void Automatition::bhopAfterCycle()
{
	if (doroBot->game->isFocused()) {
		bool isInAir = Dorobot::getInstance()->game->getPmoveCurrent()->ps->GroundEntityNum == 1023;
		if (isInAir && doroBot->strafeBot->nextFrameValues.onGround && doroBot->bindManager->bindActive("Bhop") && doroBot->uiMenu->bhop_toggle) {
			//if were going to be touching the ground on the next frame, release the jump button
			input_s* input = doroBot->game->getInput_s();
			usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

			cmd->buttons &= !USERCMD_BUTTONS_JUMP;
		}
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

void Automatition::switchOnBounce()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	Lmove lmove = Dorobot::getInstance()->game->getLmove();

	if (doroBot->game->isOnGround() || !(lmove.isLeft || lmove.isRight)) {  //should turn off if player isn't pressing anything
		bounceSwitchDirection = Direction::NONE;
		switchedOnBounce = false;
	}

	auto ps = doroBot->game->getPmoveCurrent()->ps;
	doroBot->uiDebug->addDebuginfo("pmf", ps->pm_flags);
	doroBot->uiDebug->addDebuginfo("jz", ps->JumpOriginZ);
	if (doroBot->bindManager->bindActive("Strafe switch on bounce")) {
		if (!switchedOnBounce && !doroBot->game->isOnGround() && !(ps->pm_flags & PMF_JUMPING) && ps->JumpOriginZ == 0) {
			switchedOnBounce = true;
 			bounceSwitchDirection = static_cast<Direction>(cmd->side == USERCMD_SIDE_LEFT ? USERCMD_SIDE_RIGHT : USERCMD_SIDE_LEFT);
		}
		
		if (switchedOnBounce && !doroBot->game->isOnGround() && static_cast<Direction>(cmd->side) == bounceSwitchDirection) {  //when the player has switched strafes to the opposite direction
																				  //eg. hes holding wa before bounce, script switches to wd, player notices and starts holding wd.
			bounceSwitchDirection = Direction::NONE;
		}
	}
}

void Automatition::cycleAfterStrafebot()
{
	if (doroBot->uiMenu->strafebot_toggle && doroBot->strafeBot->strafeBotCycled && !doroBot->bindManager->bindActive("Nitro")) {
		autopara();
		autoTransferzone3Spam();
		auto2Spam();
	}
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

	if (doroBot->uiMenu->switchOnBounce_toggle) {
		switchOnBounce();
	}

	if (doingAutopara || doingTransferzoneSpam || doingAuto250) {
		cmd->side = static_cast<char>(forcedDirection);
		doroBot->uiDebug->addDebuginfo("FORCED", cmd->side);
	}
	if (switchedOnBounce && bounceSwitchDirection != Direction::NONE) {  //bounce switch should override other automatition stuffs, mb change later
		cmd->side = static_cast<char>(bounceSwitchDirection);
	}
}
