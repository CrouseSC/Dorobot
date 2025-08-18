#pragma once
#include "pch.h"

class safePmove_t
{
public:
	safePmove_t()
	{
	}
	safePmove_t(pmove_t* pm)  //this will copy the pmove_t and its playerstate
	{
		constructPmove(pm);
		*this->pm->ps = *pm->ps;
	}
	safePmove_t(pmove_t* pm, playerState_s* ps)  //this will copy the pmove_t and discard its playerstate
	{
		constructPmove(pm);
		*this->pm->ps = *ps;
	}
	safePmove_t(const safePmove_t& pm)
	{
		constructPmove(pm.pm);
		*this->pm->ps = *pm.pm->ps;
	}
	~safePmove_t()
	{
		if (pm && pm->ps) {
			delete pm->ps;
			delete pm;
		}
		else if (pm) {
			delete pm;
		}
	}

	pmove_t* pm = NULL;

private:
	void constructPmove(pmove_t* pm)
	{
		this->pm = new pmove_t();
		*this->pm = *pm;
		this->pm->ps = new playerState_s();
	}
};

struct PredictionValues  //Struct containing a predicted frame's data, predicting both strafe directions
{
	Vec3<float> predictedVeloVec;  //predicted frame's velocity vector
	Vec3<float> predictedVeloIncreaseVec;  //predicted velo - last velo
	Vec3<float> predictedView;  //angles in cod's format (writable angles)
	float predictedVeloIncrease;  //predicted frame's velocity vector's 2d length - last frame's 2d length
	float optimalAngle;
	int fps;

	Vec3<float> predictedVeloVecInvert;
	Vec3<float> predictedVeloIncreaseVecInvert;
	Vec3<float> predictedViewInvert;
	float predictedVeloIncreaseInvert;
	float optimalAngleInvert;
	int fpsInvert;

	bool shotRpg;
	bool onGround;
};

class Prediction
{
public:
	Prediction(class Dorobot* doroBot);
	~Prediction();
	void PM_Accelerate(Vec3<float> wishDir, playerState_s* playerState, pml_t* pml, float wishSpeed, float accel);
	void PmoveSingle(pmove_t* pm);
	void predictMove(int fps, float angle, bool invert, safePmove_t& pmove);
	std::vector<playerState_s*> predictFramesAhead();
	std::unique_ptr<safePmove_t> buildDefaultPmove();

	playerState_s realPlayerState;  //for devmap use only, we use this to sync server/client for perfect prediction

private:
	Dorobot* doroBot;

};
