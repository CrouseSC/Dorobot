#include "pch.h"
#include "ui_debug.h"
#include "Dorobot.h"

void UI_Debug::render()
{
	std::lock_guard<std::mutex> lock(infoMutex);
	Dorobot* doroBot = Dorobot::getInstance();
	if (!doroBot->game->isConnected()) //only draw while connected to a server
		return;

	ImGui::Begin("DEBUG", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);

	for (auto& pair : info) {
		std::string text = pair.first + ": " + std::to_string(pair.second);
		ImGui::Text(text.c_str());
	}
	info.clear();
	
	ImGui::End();
}

void UI_Debug::renderAngles()
{
	input_s* input = Dorobot::getInstance()->game->getInput_s();
	usercmd_s cmd = *input->GetUserCmd(input->currentCmdNum);

	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::SetNextWindowSize(ImVec2(250, 200));
	ImGui::Begin("Angles", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::Text( ("Pitch: " + std::to_string(Dorobot::getInstance()->game->getView().x)).c_str() );
	ImGui::Text( ("Yaw: " + std::to_string(Dorobot::getInstance()->game->getView().y)).c_str() );
	ImGui::Text( ("Buttons: " + std::to_string(cmd.buttons)).c_str() );
	ImGui::Text( ("Pos X: " + std::to_string(Dorobot::getInstance()->game->getOrigin().x)).c_str() );
	ImGui::Text( ("Pos Y: " + std::to_string(Dorobot::getInstance()->game->getOrigin().y)).c_str() );
	ImGui::Text( ("Pos Z: " + std::to_string(Dorobot::getInstance()->game->getOrigin().z)).c_str() );

	ImGui::End();
}

void UI_Debug::addDebuginfo(std::string name, float value)
{
	std::lock_guard<std::mutex> lock(infoMutex);
	info.emplace(name, value);
}

void UI_Debug::addDebuginfo(std::string name, Vec3<float>* value)
{
	std::lock_guard<std::mutex> lock(infoMutex);
	std::string debugString(name + " X: " + std::to_string(value->x) + " Y: " + std::to_string(value->y) + "Z: " + std::to_string(value->z));
	info.emplace(debugString, 0);
}

UI_Debug::UI_Debug(Dorobot* doroBot)
{
	
}

UI_Debug::~UI_Debug()
{

}
