#pragma once
#include <memory>
#include <filesystem>
#include "Input.h"
#include "Render.h"
#include "ui_anglehelper.h"
#include "ui_debug.h"
#include "ui_velocity.h"
#include "ui_view.h"
#include "ui_settings.h"
#include "Game.h"
#include "imgui-docking/imgui.h"
#include "imgui-docking/imgui_internal.h"
#include "imgui-docking/backends/imgui_impl_win32.h"
#include "imgui-docking/backends/imgui_impl_dx9.h"
#include "hook_wrapper.h"
#include "memory.h"
#include "ui_90_lines.h"
#include "ui_menu.h"
#include "ui_fpswheel.h"
#include "prediction.h"
#include "strafebot.h"
#include "common_hooks.h"
#include "Automatition.h"
#include "texture_manager.h"
#include "recorder.h"
#include "positioning.h"
#include "bind_manager.h"
#include "session_manager.h"
#include "elebot.h"

extern "C" 
{
	bool __declspec(dllexport) __stdcall RIB_Main(int a, int b);
}

class Dorobot
{
public:
	void loadConfiguration();
	void saveConfiguration();
	void loadAssets();
	Dorobot();
	~Dorobot();

	std::shared_ptr<Input> input;
	std::shared_ptr<Render> render;
	std::shared_ptr<Game> game;
	std::shared_ptr<HookWrapper> hookWrapper;
	std::shared_ptr<CommonHooks> commonHooks;

	std::shared_ptr<UI_Debug> uiDebug;
	std::shared_ptr<UI_Velocity> uiVelocity;
	std::shared_ptr<UI_Anglehelper> uiAnglehelper;
	std::shared_ptr<UI_View> uiView;
	std::shared_ptr<UI_Settings> uiSettings;
	std::shared_ptr<UI_Menu> uiMenu;
	std::shared_ptr<UI_90Lines> ui90Lines;
	std::shared_ptr<UI_FpsWheel> uiFpsWheel;
	std::shared_ptr<Prediction> prediction;
	std::shared_ptr<StrafeBot> strafeBot;
	std::shared_ptr<Automatition> automatition;
	std::shared_ptr<TextureManager> textureManager;
	std::shared_ptr<Recorder> recorder;
	std::shared_ptr<Positioning> positioning;
	std::shared_ptr<BindManager> bindManager;
	std::shared_ptr<SessionManager> sessionManager;
	std::shared_ptr<Elebot> elebot;

	ImFont* toxicFont;
	ImFont* sepFont;

	bool exit = false;
	bool wantInput = false;
	bool bindToggleInput(UINT key_state);
	bool bindTpToSavedPos(UINT key_state);
	bool bindClose(UINT key_state);
	static Dorobot* dorobot;
	static Dorobot* getInstance();
	const char* CONFIG = "Dorobot/doroconfig.json";
	const char* ASSETINFO = "Dorobot/Assets/assetinfo.json";


private:
	void setupFolders();

};

