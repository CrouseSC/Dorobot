#include "pch.h"
#include "ui_90_lines.h"
#include "Dorobot.h"


void UI_90Lines::render()
{
	Dorobot* doroBot = Dorobot::getInstance();
	auto color = ImColor(doroBot->uiMenu->lines_color);
	Vec3<float> c_direction;
	std::vector<std::pair<Game::GfxPointVertex, Game::GfxPointVertex>> verts;
	for (int i = 0; i < 4; i++)
	{
		float length = 800;
		mm::angle_vectors({ 0, (float)(i * 90), 0 }, &c_direction, 0, 0);
		Vec3<float> c_point = doroBot->game->getOrigin() + (c_direction * Vec3<float>(length, length, length));
		verts.push_back({ Game::GfxPointVertex(doroBot->game->getOrigin(), color), Game::GfxPointVertex(c_point, color) });
	}

	doroBot->game->polyline(verts.size(), 1, reinterpret_cast<Game::GfxPointVertex*>(verts.data()), true);
}
UI_90Lines::UI_90Lines(Dorobot* doroBot)
{
	
}
UI_90Lines::~UI_90Lines()
{

}