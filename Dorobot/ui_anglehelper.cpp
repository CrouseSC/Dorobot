#include "pch.h"
#include "ui_anglehelper.h"

void UI_Anglehelper::render(Dorobot*& doroBot, ImVec4& color)
{
	Vec2<float> screen;
	Vec2<float> center(doroBot->game->getScreenRes().x / 2, doroBot->game->getScreenRes().y / 2);
	float fpsWheelPos = doroBot->game->getScreenRes().y / 2 + doroBot->uiMenu->fpswheel_offset_y;
	float yaw = doroBot->game->getView().y;
	float optAngle = doroBot->game->getOptimalAngle();
	float fov = doroBot->game->getFov();

	float pixelScale = (doroBot->game->getScreenRes().x / fov) * doroBot->uiMenu->ah_pixel_scale;

	screen.y = doroBot->game->getScreenRes().y / 2;
	float ahOffset = yaw - optAngle;

	screen.x = center.x + (ahOffset * pixelScale);

	float ahOffsetPixel = ahOffset * pixelScale;

	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(screen.x, -30 + screen.y), ImVec2(2.5 + screen.x, 30 + screen.y), ImColor(color));
}

void UI_Anglehelper::renderOnWheel(Dorobot*& doroBot, ImVec4& color)  //TODO: refactor into 1 func w params
{
	Vec2<float> screen;
	Vec2<float> center(doroBot->game->getScreenRes().x / 2, doroBot->game->getScreenRes().y / 2);
	float fpsWheelPos = doroBot->game->getScreenRes().y / 2 + doroBot->uiMenu->fpswheel_offset_y;
	float yaw = doroBot->game->getView().y;
	float optAngle = doroBot->game->getOptimalAngle();
	float fov = doroBot->game->getFov();

	float pixelScale = (doroBot->game->getScreenRes().x / fov) * doroBot->uiMenu->wheel_ah_pixel_scale;

	screen.y = fpsWheelPos;
	float ahOffset = yaw - optAngle;

	screen.x = center.x + (ahOffset * pixelScale);

	float ahOffsetPixel = ahOffset * pixelScale;

	ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(screen.x, screen.y), ImVec2(2.5 + screen.x, doroBot->uiMenu->fpswheel_size + screen.y), ImColor(color));
}

UI_Anglehelper::UI_Anglehelper(Dorobot* doroBot)
{
	
}

UI_Anglehelper::~UI_Anglehelper()
{

}
