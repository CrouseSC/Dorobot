#include "pch.h"
#include "strafebot.h"

StrafeBot::StrafeBot(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}

StrafeBot::~StrafeBot()
{
}

void StrafeBot::cycle()
{
	lastFps = doroBot->game->get_fps();
	if (shouldUseStrafeBot()) {
		calculateBestAngleAndFps();
		calculateBestAngleAndFps(true);
		if (!doroBot->bindManager->bindActive("Override bot")) {
			setGameToBotValues();
		}
	}
	pmove_t* pmove = doroBot->game->getPmoveCurrent();
	if (!pmove->ps)
		return;
	pmove_t* predicted = doroBot->prediction->predictMove(doroBot->game->get_fps(), doroBot->game->getView().y, false);  //run prediction even if strafebot isn't active so stuff like rpg lookdown can function
	nextFrameShotRpg = predicted->ps->WeaponDelay <= 3 && pmove->ps->WeaponDelay != 0;
	nextFrameOnGround = predicted->ps->GroundEntityNum == 1022;
	deletePmove(predicted);
}

void StrafeBot::invertStrafeAfterCycle()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	cmd->angles[1] = ANGLE2SHORT(predictedOptimalViewInvert.y);

	*reinterpret_cast<int*>(addr_maxfps) = bestFpsInvert;
}

void StrafeBot::setGameToBotValues()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	doroBot->game->setView(predictedOptimalView);
	doroBot->game->setFps(bestFps);
	cmd->angles[1] = ANGLE2SHORT(predictedOptimalView.y);  //i dont really care to reverse view->cmd conversion bs, so i just set it
}

void StrafeBot::registerBinds()
{
	doroBot->bindManager->registerBindName("15 slide", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Force 333", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Force 500", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Override bot", BIND_TYPE_HOLD);
}

bool StrafeBot::shouldUseStrafeBot()
{
	Lmove lmove = Dorobot::getInstance()->game->getLmove();
	bool isMoving = (lmove.isForward && (lmove.isRight || lmove.isLeft)) || (lmove.isRight || lmove.isLeft);
	bool isCorrectGameMode = !(doroBot->game->isNocliping() || doroBot->game->isSpectating());
	bool toggle = doroBot->uiMenu->strafebot_toggle;
	return isMoving && isCorrectGameMode && toggle && doroBot->game->getPmoveCurrent()->ps;
}

std::vector<int> StrafeBot::getFpsList()
{
	std::vector<int> fpsList;
	if (!doroBot->uiMenu->no200_toggle) {
		fpsList = { 333, 250, 200, 125, 15 };
	}
	else {
		fpsList = { 333, 250, 125, 15 };
	}

	if (doroBot->bindManager->bindActive("Force 333")) {
		fpsList = { 333 };
	}
	else if (doroBot->bindManager->bindActive("Force 500")) {
		fpsList = { 500 };
	}
	return fpsList;
}

void StrafeBot::calculateBestAngleAndFps(bool invert)
{
	std::vector<int> fpsList = getFpsList();
	pmove_t* pmove = doroBot->game->getPmoveCurrent();

	int i;
	for (i = 0; i < 10; i++) {
		float gSpeed;
		if (doroBot->game->isOnGround()) {
			gSpeed = 231.7f;
		}
		else {
			gSpeed = 189.4f - i*0.1f;
		}

		float approxOptimal = doroBot->game->getOptimalAngle(gSpeed, invert);

		Vec3<float> currentVeloVec = pmove->ps->velocity;
		float currentVelo = currentVeloVec.Length2D();
		float bestPredictedVelo = 0;
		float bestPredictedVeloIncrease = 0;
		Vec3<float> bestPredictedVeloVec;
		if (!invert) {
			bestFps = fpsList[0];
		}
		else {
			bestFpsInvert = fpsList[0];
		}

		if (!invert) {
			predictedOptimalView = doroBot->game->getView();
			predictedOptimalView.y = approxOptimal;
			predictedOptimalView = doroBot->game->toCodAngles(predictedOptimalView);
		}
		else {
			predictedOptimalViewInvert = doroBot->game->getView();
			predictedOptimalViewInvert.y = approxOptimal;
			predictedOptimalViewInvert = doroBot->game->toCodAngles(predictedOptimalViewInvert);
		}

		if (doroBot->game->isOnGround()) {
			if (!invert) {
				bestFps = 333;
			}
			else {
				bestFpsInvert = 333;
			}
			return;
		}

		for (auto fps : fpsList) {
			if (fps == 15 && !doroBot->bindManager->bindActive("15 slide")) {
				continue;
			}

			pmove_t* currentPmove = doroBot->prediction->predictMove(fps, approxOptimal, invert);
			Vec3<float> predictedVeloVec = currentPmove->ps->velocity;
			float predictedVelo = predictedVeloVec.Length2D();
			deletePmove(currentPmove);

			float veloIncrease = predictedVelo - currentVelo;
			Vec3<float> predictedVeloChangeVec = predictedVeloVec - currentVeloVec;
			if (veloIncrease * fps > bestPredictedVeloIncrease * (invert ? bestFpsInvert : bestFps) ) {
				bestPredictedVelo = predictedVelo;
				bestPredictedVeloIncrease = veloIncrease;
				bestPredictedVeloVec = predictedVeloVec;
				doroBot->uiDebug->addDebuginfo("bestPredictedVeloIncrease", bestPredictedVeloIncrease);
				if (!invert) {
					bestFps = fps;
				}
				else {
					bestFpsInvert = fps;
				}
			}
			else if (i > 6 && doroBot->game->getVelocity().Length2D() >= 2400.f
				&& veloIncrease <= 0 && mm::truncate_vector(predictedVeloChangeVec) != Vec2<float>(0.f, 0.f)) {
				bestPredictedVelo = predictedVelo;
				bestPredictedVeloIncrease = veloIncrease;
				bestPredictedVeloVec = predictedVeloVec;
				if (!invert) {
					bestFps = fps;
				}
				else {
					bestFpsInvert = fps;
				}
			}
		}

		if (!invert) {
			predictedVeloIncreaseVec = bestPredictedVeloVec - currentVeloVec;
			this->predictedVeloIncrease = bestPredictedVeloIncrease;
			if (bestPredictedVeloIncrease > 0) {
				break;
			}
		}
		else {
			predictedVeloIncreaseVecInvert = bestPredictedVeloVec - currentVeloVec;
			this->predictedVeloIncreaseInvert = bestPredictedVeloIncrease;
			if (bestPredictedVeloIncrease > 0) {
				break;
			}
		}
	}
	doroBot->uiDebug->addDebuginfo("fps", bestFps);
	doroBot->uiDebug->addDebuginfo("veloIncreaseVec", &predictedVeloIncreaseVec);

}
