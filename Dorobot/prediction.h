#pragma once
#include "pch.h"

class Prediction
{
public:
	Prediction(class Dorobot* doroBot);
	~Prediction();
	void PM_Accelerate(Vec3<float> wishDir, playerState_s* playerState, pml_t* pml, float wishSpeed, float accel);
	void PmoveSingle(pmove_t* pm);
	pmove_t* predictMove(int fps, float angle, bool invert);

private:
	Dorobot* doroBot;

};
