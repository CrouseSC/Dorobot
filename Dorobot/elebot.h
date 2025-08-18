#pragma once
#include "pch.h"

struct TraceResults
{
	trace_t trace;
	Vec3<float> hitposAdjusted;
	Vec3<float> hitposReal;
};

class Elebot
{
public:
	Elebot(Dorobot* doroBot);
	~Elebot();
	void cycle();
	void renderMarkers();
	void elevate();
	bool doingEle = false;
	void setupAnimation();
	void registerBinds();

private:
	Dorobot* doroBot;
	Animation doroWalk;
	TraceResults callCGTrace();
	TraceResults traceResults;
	bool animSetup = false;

};
