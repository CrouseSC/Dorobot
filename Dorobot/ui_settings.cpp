#include "pch.h"
#include "ui_settings.h"
#include "Dorobot.h"


void UI_Settings::render()
{

}
UI_Settings::UI_Settings(Dorobot* doroBot)
{
	doroBot->render->addCallback([this]() { this->render(); });

}
UI_Settings::~UI_Settings()
{

}