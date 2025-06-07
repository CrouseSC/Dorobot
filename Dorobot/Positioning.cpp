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

void Positioning::turnToPosition(const Vec2<float>& position, float Difference)
{
	Vec2<float> currentPos = mm::truncate_vector(doroBot->game->getOrigin());
	Vec2<float> newOrigin(position.x * -1.f + currentPos.x, position.y * -1.f + currentPos.y);

	float newYaw = mm::normalise(mm::tilt_angle(newOrigin) + 180.f, 0.f, 360.f);  //Not sure why you have to add 180, but ohwell
	Vec3<float> normalView = doroBot->game->getView();
	normalView.y = mm::normalise(newYaw + Difference, 0.f, 360.f);
	doroBot->game->setNormalView(normalView);
}
