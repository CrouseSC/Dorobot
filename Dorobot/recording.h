#pragma once
#include "pch.h"
	
class CommandPacket
{
public:
	CommandPacket(const usercmd_s& cmd, const Vec3<float>& origin, const Vec3<float>& velocity, const Vec3<float>& normalView) :
		cmd(cmd), origin(origin), velocity(velocity), normalView(normalView)
	{
	}

	CommandPacket(){};

	Vec3<float> origin;  //position when packet was sent
	Vec3<float> velocity;
	Vec3<float> normalView;
	usercmd_s cmd;
};

struct Recording
{
	std::string author;
	std::string mapName;
	std::string name;
	std::vector<CommandPacket> packets;
};
