#pragma once
#include "vectors.h"
#include "texture_wrapper.h"

enum UI_TABS
{
	MOVEMENT, RECORDER, RUNS, MISC, BINDS
};

class UI_Menu
{
	public:
	UI_Menu(class Dorobot* doroBot);
	~UI_Menu();
	void menu(Dorobot* doroBot);
	void render();

	bool debugToggle = false;
	bool velo_meter = false;
	bool sep_velo = false;
	ImVec4 color = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 lines_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	ImVec4 anglehelper_color = { 0.0f, 1.0f, 0.0f, 1.0f };
	float velo_scale = 1.5;
	bool lock_velo_pos = true;
	bool anglehelper_toggle = false;
	bool fpswheel_toggle = false;
	bool fpswheel_centerline_toggle = false;
	float fpswheel_size = 20.f;
	float fpswheel_offset_y = 0.f;
	float fpswheel_offset_x = 1.f;
	float ah_pixel_scale = 1.f;
	float wheel_ah_pixel_scale = 1.f;
	float wheel_pixel_scale = 1.f;
	bool lines_toggle = false;
	bool pfps_toggle = false;
	bool strafebot_toggle = false;
	bool autofps_toggle = false;
	bool keyOnW_toggle = false;
	bool keyOnWWDSelected = true;
	bool sprintToPrestrafe_toggle = false;
	bool sprintToPrestrafeIsWD = false;
	bool jumpAtVelo_toggle = false;
	float veloToJumpAt = 359;
	bool autopara_toggle = false;
	bool auto250_toggle = false;
	bool autoparaOnlyOnce = true;
	bool autoTransferzone3Spam_toggle = false;
	bool autoTransferzone3SpamOnlyOnce = true;
	bool rpgLookdown_toggle = false;
	bool rpgLookdownBackwards = false;
	float rpgLookdownPitch = 85.f;
	float rpgLookdownYaw = 85.f;
	bool jumpOnRpg_toggle = false;
	bool displayAngles_toggle = false;
	bool bhop_toggle = false;
	bool yawScript_toggle = false;
	bool no200_toggle = false;
	bool progressbar_toggle = true;
	float yawScriptAngle = 0.f;

	// Default the position to the center of the screen if there is no position in the config file
	Vec2<float> velo_pos = Vec2<float>(GetSystemMetrics(SM_CXSCREEN) / 2, GetSystemMetrics(SM_CYSCREEN) / 2);

	std::string copied_position;
	
	Vec2<float> pos1;
	Vec2<float> pos2;
	Vec2<float> pos3;

	bool isEditing = false;
	int selectedFrame = 1;
	Vec2<int> trimFrames;

	int recordingsSelectedItem;

	char nameInputText[32] = { 0 };

private:
	void ImGuiImageButton(const std::string& label, const ImVec2& size, const TextureWrapper* texture);
	void autofixRecording();
	UI_TABS selectedTab = MOVEMENT;
	int capturingInputForBind = -1;

};
