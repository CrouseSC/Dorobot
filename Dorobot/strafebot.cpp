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
	strafeBotCycled = shouldUseStrafeBot();

	if (strafeBotCycled) {
		nextFrameValues = calculateBestAngleAndFpsForBothDirections(*doroBot->prediction->buildDefaultPmove().get());
		if (!doroBot->bindManager->bindActive("Override bot")) {
			setGameToBotValues();
		}
	}
	pmove_t* pmove = doroBot->game->getPmoveCurrent();
	if (!pmove->ps)
		return;

	auto predicted = doroBot->prediction->buildDefaultPmove();
	doroBot->prediction->predictMove(doroBot->game->get_fps(), doroBot->game->getView().y, false, *predicted.get());  //run prediction even if strafebot isn't active so stuff like rpg lookdown can function
	nextFrameValues.shotRpg = predicted->pm->ps->WeaponDelay <= 3 && pmove->ps->WeaponDelay != 0;
	nextFrameValues.onGround = predicted->pm->ps->GroundEntityNum == 1022;
}

void StrafeBot::cycleBeforeAutomatition()
{
	doTilt();
}

void StrafeBot::cycleBeforeWritePacket()
{
	playerState_s* ps = (playerState_s*)(addr_playerState);
	input_s* input = Dorobot::getInstance()->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (Dorobot::getInstance()->bindManager->bindActive("Nitro") && Dorobot::getInstance()->strafeBot->strafeBotCycled
		&& Dorobot::getInstance()->game->getVelocity().Length2D() >= 100.f) {

		float newPitch;
		if (ps->GroundEntityNum == 1022) {
			newPitch = 85.f;
		}
		else {
			if (Dorobot::getInstance()->game->getVelocity().Length2D() > 2300.f) {
				newPitch = 39.f;
			}
			else {
				newPitch = 50.f;
			}
		}
		cmd->angles[0] = ANGLE2SHORT(Dorobot::getInstance()->game->toCodAngles(Vec3<float>(newPitch, 0, 0)).x);
		if (!Dorobot::getInstance()->game->isDevmap()) {
			auto deltaAngles = Dorobot::getInstance()->game->getDeltaAngles();
		}
	}
}

void StrafeBot::cycleAfterPredictPlayerState()
{
	playerState_s* ps = (playerState_s*)(addr_playerState);

	if (Dorobot::getInstance()->bindManager->bindActive("Nitro") && Dorobot::getInstance()->strafeBot->strafeBotCycled
		&& Dorobot::getInstance()->game->getVelocity().Length2D() >= 300.f) {
		Dorobot::getInstance()->uiDebug->addDebuginfo("NEWPITCH", ps->ViewAngles.x);
		ps->ViewAngles.x = tiltOldPitch;
		ps->ViewAngles.y = Dorobot::getInstance()->game->getOptimalAngle();
	}

	ps->ViewAngles.z = 0;
}

void StrafeBot::invertStrafeAfterCycle()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	cmd->angles[1] = ANGLE2SHORT(nextFrameValues.predictedViewInvert.y);

	*reinterpret_cast<int*>(addr_maxfps) = nextFrameValues.fpsInvert;
}

void StrafeBot::setGameToBotValues()
{
	input_s* input = doroBot->game->getInput_s();
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);

	if (doroBot->uiMenu->strafebot_toggle) {
		doroBot->game->setView(nextFrameValues.predictedView);
		cmd->angles[1] = ANGLE2SHORT(nextFrameValues.predictedView.y);  //i dont really care to reverse view->cmd conversion bs, so i just set it
	}
	if (doroBot->uiMenu->autofps_toggle) {
		doroBot->game->setFps(nextFrameValues.fps);
	}
}

void StrafeBot::registerBinds()
{
	doroBot->bindManager->registerBindName("15 slide", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Force 333", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Force 500", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Override bot", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Nitro", BIND_TYPE_HOLD);
}

bool StrafeBot::shouldUseStrafeBot()
{
	Lmove lmove = Dorobot::getInstance()->game->getLmove();
	bool isMoving = (lmove.isForward && (lmove.isRight || lmove.isLeft)) || (lmove.isRight || lmove.isLeft);
	bool isCorrectGameMode = !(doroBot->game->isNocliping() || doroBot->game->isSpectating());
	return isMoving && isCorrectGameMode && doroBot->game->getPmoveCurrent()->ps;
}

void StrafeBot::doTilt()
{
	input_s* input = (input_s*)0xCC4FF8;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	playerState_s* ps = (playerState_s*)(addr_playerState);
	tiltOldPitch = ps->ViewAngles.x;
	if (Dorobot::getInstance()->bindManager->bindActive("Nitro") && Dorobot::getInstance()->game->getVelocity().Length2D() >= 300.f) {

		float newPitch;
		if (ps->GroundEntityNum == 1022) {
			newPitch = 85.f;
		}
		else {
			if (doroBot->game->getVelocity().Length2D() > 2300.f) {
				newPitch = 39.f;
			}
			else {
				newPitch = 50.f;
			}
		}

		cmd->angles[0] = ANGLE2SHORT(Dorobot::getInstance()->game->toCodAngles(Vec3<float>(newPitch, 0, 0)).x);

		float tiltValue = ps->GroundEntityNum == 1022 ? 84.f : Dorobot::getInstance()->uiMenu->tiltValue;
		if (Dorobot::getInstance()->game->getLmove().isLeft) {
			tiltValue = abs(tiltValue);
		}
		else {
			tiltValue = -abs(tiltValue);
		}

		float tiltValueCod = Dorobot::getInstance()->game->toCodAngles(Vec3<float>(0, 0, tiltValue)).z;
		cmd->angles[2] = ANGLE2SHORT(tiltValueCod);
	}
	else {
		cmd->angles[2] = ANGLE2SHORT(Dorobot::getInstance()->game->toCodAngles(Vec3<float>(0, 0, 0)).z);
	}
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

PredictionValues StrafeBot::calculateBestAngleAndFps(safePmove_t pmove, bool invert)
{
	PredictionValues predictionValues;
	std::vector<int> fpsList = getFpsList();

	if (pmove.pm->ps->GroundEntityNum == 1022 && !doroBot->automatition->isJumping) {  //air calcs apply on the frame the player is jumping but isn't in air yet
		if (!invert) {
			predictionValues = calculateBestAngleForGroundStrafe(pmove);
		}
		return predictionValues;
	}

	int i;
	for (i = 0; i < 10; i++) {
		float gSpeed;
		if (doroBot->game->isDevmap()) {  //190.f doesn't work very well if deltaAngles are mismatched between the client and server.
			gSpeed = 190.f - i * 0.1f;
		}
		else {
			gSpeed = 189.4f - i * 0.1f;
		}

		float approxOptimal;
		if (doroBot->uiMenu->strafebot_toggle) {
			approxOptimal = doroBot->game->getOptimalAngle(gSpeed, invert);
		}
		else {
			approxOptimal = doroBot->game->getView().y;
		}

		if (doroBot->bindManager->bindActive("Nitro") && Dorobot::getInstance()->game->getVelocity().Length2D() >= 300.f) {
			if (doroBot->game->getLmove().isLeft) {
				approxOptimal += (10.f + i * 1.f);
			}
			else {
				approxOptimal -= (10.f + i * 1.f);
			}
		}

		Vec3<float> currentVeloVec = pmove.pm->ps->velocity;
		float currentVelo = currentVeloVec.Length2D();
		float bestPredictedVelo = 0;
		float bestPredictedVeloIncrease = 0;
		Vec3<float> bestPredictedVeloVec;
		if (!invert) {
			predictionValues.fps = fpsList[0];
			predictionValues.predictedView = doroBot->game->getView();
			predictionValues.predictedView.y = approxOptimal;
			predictionValues.predictedView = doroBot->game->toCodAngles(predictionValues.predictedView);
		}
		else {
			predictionValues.fpsInvert = fpsList[0];
			predictionValues.predictedViewInvert = doroBot->game->getView();
			predictionValues.predictedViewInvert.y = approxOptimal;
			predictionValues.predictedViewInvert = doroBot->game->toCodAngles(predictionValues.predictedViewInvert);
		}

		for (auto fps : fpsList) {
			safePmove_t pmoveCurrent(pmove);
			if (fps == 15 && !doroBot->bindManager->bindActive("15 slide")) {
				continue;
			}

			doroBot->prediction->predictMove(fps, approxOptimal, invert, pmoveCurrent);
			Vec3<float> predictedVeloVec = pmoveCurrent.pm->ps->velocity;
			float predictedVelo = predictedVeloVec.Length2D();

			float veloIncrease = predictedVelo - currentVelo;
			Vec3<float> predictedVeloChangeVec = predictedVeloVec - currentVeloVec;
			if (veloIncrease * fps > bestPredictedVeloIncrease * (invert ? predictionValues.fpsInvert : predictionValues.fps) ) {
				bestPredictedVelo = predictedVelo;
				bestPredictedVeloIncrease = veloIncrease;
				bestPredictedVeloVec = predictedVeloVec;
				doroBot->uiDebug->addDebuginfo("bestPredictedVeloIncrease", bestPredictedVeloIncrease);

				if (!invert) {
					predictionValues.fps = fps;
				}
				else {
					predictionValues.fpsInvert = fps;
				}
			}
			else if (i > 6 && doroBot->game->getVelocity().Length2D() >= 2400.f
				&& veloIncrease <= 0 && mm::truncate_vector(predictedVeloChangeVec) != Vec2<float>(0.f, 0.f)) {
				bestPredictedVelo = predictedVelo;
				bestPredictedVeloIncrease = veloIncrease;
				bestPredictedVeloVec = predictedVeloVec;

				if (!invert) {
					predictionValues.fps = fps;
				}
				else {
					predictionValues.fpsInvert = fps;
				}
			}
		}

		if (!invert) {
			predictionValues.predictedVeloIncreaseVec = bestPredictedVeloVec - currentVeloVec;
			predictionValues.predictedVeloIncrease = bestPredictedVeloIncrease;
			if (bestPredictedVeloIncrease > 0) {
				break;
			}
		}
		else {
			predictionValues.predictedVeloIncreaseVecInvert = bestPredictedVeloVec - currentVeloVec;
			predictionValues.predictedVeloIncreaseInvert = bestPredictedVeloIncrease;
			if (bestPredictedVeloIncrease > 0) {
				break;
			}
		}
	}

	doroBot->uiDebug->addDebuginfo("fps", predictionValues.fps);
	doroBot->uiDebug->addDebuginfo("veloIncreaseVec", &predictionValues.predictedVeloIncreaseVec);
	return predictionValues;
}

PredictionValues StrafeBot::calculateBestAngleForGroundStrafe(safePmove_t pmove, bool invert)
{
	PredictionValues predictionValues;

	int i;
	int besti = 0;
	float bestPredictedVeloIncrease = -5.f;
	float bestPredictedVelo = 0;

	for (i = 0; i < 30; i++) {
		safePmove_t pmoveCurrent(pmove);
		float gSpeed = 232.7f - i * 0.2f;

		float approxOptimal;
		if (doroBot->uiMenu->strafebot_toggle) {
			approxOptimal = doroBot->game->getOptimalAngle(gSpeed);
		}
		else {
			approxOptimal = doroBot->game->getView().y;
		}

		if (pmoveCurrent.pm->ps->velocity.Length2D() >= 300.f && doroBot->bindManager->bindActive("Nitro") && Dorobot::getInstance()->game->getVelocity().Length2D() >= 300.f) {
			if (Dorobot::getInstance()->game->getLmove().isLeft) {
				approxOptimal += (10.f + i * 1.f);
			}
			else {
				approxOptimal -= (10.f + i * 1.f);
			}
		}

		Vec3<float> currentVeloVec = pmoveCurrent.pm->ps->velocity;
		float currentVelo = currentVeloVec.Length2D();
		Vec3<float> bestPredictedVeloVec;
		predictionValues.fps = 333;
		predictionValues.fpsInvert = 333;

		doroBot->prediction->predictMove(predictionValues.fps, approxOptimal, false, pmoveCurrent);
		Vec3<float> predictedVeloVec = pmoveCurrent.pm->ps->velocity;
		float predictedVelo = predictedVeloVec.Length2D();

		float veloIncrease = predictedVelo - currentVelo;
		Vec3<float> predictedVeloChangeVec = predictedVeloVec - currentVeloVec;
		if (veloIncrease > bestPredictedVeloIncrease) {
			bestPredictedVelo = predictedVelo;
			bestPredictedVeloIncrease = veloIncrease;
			bestPredictedVeloVec = predictedVeloVec;
			predictionValues.predictedVeloIncreaseVec = bestPredictedVeloVec - currentVeloVec;
			predictionValues.predictedVeloIncrease = bestPredictedVeloIncrease;
			predictionValues.predictedView = doroBot->game->getView();
			predictionValues.predictedView.y = approxOptimal;
			predictionValues.predictedView = doroBot->game->toCodAngles(predictionValues.predictedView);
			besti = i;
		}
	}

	return predictionValues;
}

PredictionValues StrafeBot::calculateBestAngleAndFpsForBothDirections(const safePmove_t& pmove)
{
	PredictionValues predictionValues = calculateBestAngleAndFps(pmove);
	PredictionValues predictionValuesInvert = calculateBestAngleAndFps(pmove, true);
	predictionValues.fpsInvert = predictionValuesInvert.fpsInvert;
	predictionValues.optimalAngleInvert = predictionValuesInvert.optimalAngleInvert;
	predictionValues.predictedVeloIncreaseInvert = predictionValuesInvert.predictedVeloIncreaseInvert;
	predictionValues.predictedVeloIncreaseVecInvert = predictionValuesInvert.predictedVeloIncreaseVecInvert;
	predictionValues.predictedVeloVecInvert = predictionValuesInvert.predictedVeloVecInvert;
	predictionValues.predictedViewInvert = predictionValuesInvert.predictedViewInvert;

	return predictionValues;
}
