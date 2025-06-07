#include "pch.h"
#include "ui_view.h"
#include "Dorobot.h"

void UI_View::render()
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (!doroBot->game->isConnected()) //only draw while connected to a server
		return;
	Vec3<float> view = doroBot->game->getView();
	
	/*ImGui::SetNextWindowBgAlpha(.2);
	ImGui::Begin("View information", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::BeginTable("view info", 2);
		ImGui::TableNextColumn();
		ImGui::Text("Pitch:");
		ImGui::TableNextColumn();
		ImGui::Text("%f", view.x);

		ImGui::TableNextColumn();
		ImGui::Text("Yaw:");
		ImGui::TableNextColumn();
		ImGui::Text("%f", view.y);

		ImGui::TableNextColumn();
		ImGui::Text("Roll:");
		ImGui::TableNextColumn();
		ImGui::Text("%f", view.z);
	ImGui::EndTable();
	ImGui::End();*/
}
UI_View::UI_View(Dorobot* doroBot)
{
	
}
UI_View::~UI_View()
{

}