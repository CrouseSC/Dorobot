#include "pch.h"
#include "Positioning.h"

Positioning::Positioning(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}

void Positioning::startMoveForward(short amount)
{
	input_s* input = (input_s*)0xCC4FF8;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	cmd->forward = amount;
}

void Positioning::startMoveForwardHidden(short amount)
{
	input_s* input = (input_s*)0xCC4FF8;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	usercmd_s* oldcmd = input->GetUserCmd(input->currentCmdNum - 1);
	oldcmd->forward = amount;
	oldcmd->serverTime = cmd->serverTime - 1;
}

void Positioning::startMoveRight(short amount)
{
	input_s* input = (input_s*)addr_usercmd;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	cmd->side = amount;
}


void Positioning::endMoveForward()
{
	usercmd_s* cmd = doroBot->game->getInput_s()->GetUserCmd(doroBot->game->getInput_s()->currentCmdNum);
	cmd->forward = 0;
}

bool Positioning::moveToPosition(const Vec2<float>& position, float errorRoom)
{
	//i'll rewrite this. eventually.
	Vec2<float> currentPos = mm::truncate_vector(doroBot->game->getOrigin());
	float dist = currentPos.Dist(position);
	static int frameCounter = 0;
	constexpr int PER_FRAMES = 1;
	static bool doneForward{};

	if (dist >= errorRoom) {
		if ((dist < 27.5f && doneForward) || (dist < 2.5f)) {
			doroBot->game->setFps(500);
			if ((doroBot->game->getVelocity().Length2D() == 0) && (frameCounter % PER_FRAMES == 0)) {
				turnToPosition(position, 180);
				startMoveForward(129);
				if (doroBot->game->getVelocity().Length2D() < 10)
					doneForward = false;
			}
		}
		else {
			doroBot->game->setFps(125);
			turnToPosition(position, 0.f);
			startMoveForward(127);
			if (doroBot->game->getVelocity().Length2D() > 150)
				doneForward = true;
		}
	}
	frameCounter++;
	if (dist < errorRoom && (doroBot->game->getVelocity().Length2D() == 0) && (frameCounter % PER_FRAMES == 0)) {
		doneForward = false;
		frameCounter = 0;
		return true;
	}
	else {
		return false;
	}
}

float Positioning::axisDist(const Vec3<float>& v1, const Vec3<float>& v2, Axis axis)
{
	return axis == AXIS_X ? v2.x - v1.x : v2.y - v1.y;
}

float Positioning::axisDist(const Vec2<float>& v1, const Vec2<float>& v2, Axis axis)
{
	return axis == AXIS_X ? v2.x - v1.x : v2.y - v1.y;
}

float getAxis(const Vec3<float>& v, Axis axis)
{
	return axis == AXIS_X ? v.x : v.y;
}

bool Positioning::moveToElePosition(const Vec2<float>& position, Axis axis)
{
	constexpr int STEP_2_FPS = 333;
	Vec2<float> currentPos = mm::truncate_vector(doroBot->game->getOrigin());
	usercmd_s* cmd = doroBot->game->getInput_s()->GetUserCmd(doroBot->game->getInput_s()->currentCmdNum);
	playerState_s* ps = (playerState_s*)(addr_playerState);

	float dist = axisDist(position, currentPos, axis);
	float angle = axis == AXIS_X ? 0.f : 90.f;
	if (dist > 0.f) {
		angle = angle + 180.f;
	}

	//should move towards wall if we're not exactly (0.125) away
	//if (GetAsyncKeyState(VK_XBUTTON1) < 0) {
		if (fabsf(dist) != 0.125 && fabsf(dist) < 0.128 && fabsf(dist) > 0.123) {
			cmd->angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0, angle, 0)).y);
			cmd->forward = USERCMD_FORWARD_FORWARD;
			return false;
		}
	//}

	//hugging wall?
	unsigned char prevForward = cmd->forward;
	cmd->forward = USERCMD_FORWARD_FORWARD;
	safePmove_t pmove = *doroBot->prediction->buildDefaultPmove();
	doroBot->prediction->predictMove(43, angle, false, pmove);
	cmd->forward = prevForward;
	if (fabsf(getAxis(pmove.pm->ps->velocity, axis)) < 1.f) {
		return false;
	}

	//shouldn't do anything if we're far away
	if (fabsf(dist) > 0.125) {
		return false;
	}

	/*static int held = 0;
	if (GetAsyncKeyState(VK_XBUTTON1) >= 0) {
		return false;
	}*/
	
	dist = fabsf(dist);
	doroBot->uiDebug->addDebuginfo("angleToEle", angle);

	int step = dist < 0.07f ? 2 : 1;

	int bestFps = step == 1 ? 1000 : STEP_2_FPS;
	float bestDist = dist;
	Vec3<float> newPos;
	float bestAngle = angle;

	//predictmove relies on our current usercmd, should parameterize that later
	cmd->forward = USERCMD_FORWARD_FORWARD;

	int eleSteps = step == 1 ? 90 : 180;
	std::vector<int> fpsValues = {};

	if (step == 1) {
		for (int i = 1; i < 25; i++) {
			fpsValues.push_back(1000.f / i);
		}
	}
	else {
		fpsValues.push_back(STEP_2_FPS);
	}

	for (int i = 1; i < eleSteps; i++) {
		for (int fps : fpsValues) {
			float eleAngle = angle + i * 1.f;
			safePmove_t pmove = *doroBot->prediction->buildDefaultPmove();
			doroBot->prediction->predictMove(fps, eleAngle, false, pmove);
			float dist = axisDist(Vec2<float>(pmove.pm->ps->origin.x, pmove.pm->ps->origin.y), position, axis);
			if (fabsf(dist) < fabsf(bestDist)) {
				bestDist = dist;
				bestFps = fps;
				bestAngle = eleAngle;
				newPos = pmove.pm->ps->origin;
			}
		}
	}

	Vec3<float> normalView = doroBot->game->getView();
	normalView.y = bestAngle;
	cmd->serverTime = ps->commandTime + 1000.f / bestFps;
	cmd->angles[1] = ANGLE2SHORT(doroBot->game->toCodAngles(Vec3<float>(0, bestAngle, 0)).y);
	//doroBot->game->setNormalView(normalView);

	/*doroBot->game->addObituary(("MOVED FPS " + std::to_string(bestFps) + " DIST " + std::to_string(bestDist)
		+ " POSX " + std::to_string(newPos.x) + " POSY " + std::to_string(newPos.y) + " BESTANGLE " + std::to_string(bestAngle)).c_str());*/

	return true;
}

void Positioning::turnToPosition(const Vec2<float>& position, float Difference)
{
	Vec2<float> currentPos = mm::truncate_vector(doroBot->game->getOrigin());
	Vec2<float> newOrigin(position.x * -1.f + currentPos.x, position.y * -1.f + currentPos.y);

	float newYaw = mm::normalise(mm::tilt_angle(newOrigin) + 180.f, 0.f, 360.f);  //Not sure why you have to add 180, but ohwell
	Vec3<float> normalView = doroBot->game->getView();
	normalView.y = mm::normalise(newYaw + Difference, 0.f, 360.f);
	doroBot->game->setNormalView(normalView);
}
