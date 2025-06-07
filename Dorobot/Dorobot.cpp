#include "pch.h"
#include "Dorobot.h"
#include <include/json.hpp>

using json = nlohmann::json;
static const char* CONFIG = "doroconfig.json";

//pretend its an audio codec for the miles sound system
bool __stdcall RIB_Main(int a, int b)
{
	return true;
}

Dorobot* Dorobot::dorobot = nullptr;
bool Dorobot::bindToggleInput(UINT key_state)
{
	if (key_state == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (key_state == WM_KEYUP)
	{
		wantInput = !wantInput;
		return true;
	}
}

bool Dorobot::bindTpToSavedPos(UINT key_state)
{
	if (key_state == WM_KEYDOWN) //return true on key down just so nothing else receives the key down stroke
		return true; 
	if (key_state == WM_KEYUP)
	{
		if(uiMenu->copied_position != "" && game->isConnected())
			game->sendCommandToConsole(("setviewpos " + uiMenu->copied_position).c_str());
		return true;
	}
}

void Dorobot::loadConfiguration() {
    std::ifstream file(CONFIG);
    if (!file.is_open()) { saveConfiguration(); return; }

    json j;
    try {
        file >> j;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to parse config: " << e.what() << std::endl;
        saveConfiguration();
        return;
    }

    uiMenu->velo_meter = j.value("Speedometer", uiMenu->velo_meter);
    uiMenu->sep_velo = j.value("SepVelo", uiMenu->sep_velo);
    uiMenu->debugToggle = j.value("PosHud", uiMenu->debugToggle);
    uiMenu->anglehelper_toggle = j.value("Anglehelper", uiMenu->anglehelper_toggle);
    uiMenu->lines_toggle = j.value("90_Lines", uiMenu->lines_toggle);
    uiMenu->fpswheel_toggle = j.value("FPSWheel", uiMenu->fpswheel_toggle);
    uiMenu->fpswheel_centerline_toggle = j.value("FPSWheel_centerline", uiMenu->fpswheel_centerline_toggle);
    uiMenu->pfps_toggle = j.value("pfps", uiMenu->pfps_toggle);
    uiMenu->strafebot_toggle = j.value("strafebot", uiMenu->strafebot_toggle);
    uiMenu->keyOnW_toggle = j.value("keyOnW_toggle", uiMenu->keyOnW_toggle);
    uiMenu->keyOnWWDSelected = j.value("keyOnWWDSelected", uiMenu->keyOnWWDSelected);
    uiMenu->sprintToPrestrafe_toggle = j.value("sprintToPrestrafe", uiMenu->sprintToPrestrafe_toggle);
    uiMenu->sprintToPrestrafeIsWD = j.value("sprintToPrestrafeIsWD", uiMenu->sprintToPrestrafeIsWD);
    uiMenu->jumpAtVelo_toggle = j.value("jumpAtVelo", uiMenu->jumpAtVelo_toggle);
    uiMenu->velo_scale = j.value("Scale", uiMenu->velo_scale);
    uiMenu->fpswheel_offset_y = j.value("FPSWheelOffsetY", uiMenu->fpswheel_offset_y);
    uiMenu->fpswheel_offset_x = j.value("FPSWheelOffsetX", uiMenu->fpswheel_offset_x);
    uiMenu->fpswheel_size = j.value("FPSWheelSize", uiMenu->fpswheel_size);
    uiMenu->ah_pixel_scale = j.value("Anglehelper_pixel_scale", uiMenu->ah_pixel_scale);
    uiMenu->wheel_ah_pixel_scale = j.value("Wheel_anglehelper_pixel_scale", uiMenu->wheel_ah_pixel_scale);
    uiMenu->wheel_pixel_scale = j.value("Wheel_pixel_scale", uiMenu->wheel_pixel_scale);
    uiMenu->veloToJumpAt = j.value("veloToJumpAt", uiMenu->veloToJumpAt);
    uiMenu->autopara_toggle = j.value("autopara_toggle", uiMenu->autopara_toggle);
    uiMenu->auto250_toggle = j.value("auto250Toggle", uiMenu->auto250_toggle);
    uiMenu->autoparaOnlyOnce = j.value("autoparaOnlyOnce", uiMenu->autoparaOnlyOnce);
    uiMenu->autoTransferzone3Spam_toggle = j.value("autoTransferzone3Spam", uiMenu->autoTransferzone3Spam_toggle);
    uiMenu->autoTransferzone3SpamOnlyOnce = j.value("autoTransferzone3SpamOnlyOnce", uiMenu->autoTransferzone3SpamOnlyOnce);
    uiMenu->rpgLookdown_toggle = j.value("rpgLookdown", uiMenu->rpgLookdown_toggle);
    uiMenu->rpgLookdownPitch = j.value("rpgLookdownPitch", uiMenu->rpgLookdownPitch);
    uiMenu->rpgLookdownYaw = j.value("rpgLookdownYaw", uiMenu->rpgLookdownYaw);
    uiMenu->rpgLookdownBackwards = j.value("rpgLookdownBackwards", uiMenu->rpgLookdownBackwards);
    uiMenu->jumpOnRpg_toggle = j.value("jumpOnRpg", uiMenu->jumpOnRpg_toggle);
    uiMenu->displayAngles_toggle = j.value("displayAnglesToggle", uiMenu->displayAngles_toggle);
    uiMenu->bhop_toggle = j.value("bhopToggle", uiMenu->bhop_toggle);
    uiMenu->yawScript_toggle = j.value("yawScriptToggle", uiMenu->yawScript_toggle);
    uiMenu->yawScriptAngle = j.value("yawScriptAngle", uiMenu->yawScriptAngle);
    uiMenu->no200_toggle = j.value("no200Toggle", uiMenu->no200_toggle);

	bindManager->binds = j.value("binds", bindManager->binds);

    if (j.contains("Position") && j["Position"].is_array() && j["Position"].size() == 2) {
        uiMenu->velo_pos.x = j["Position"][0].get<float>();
        uiMenu->velo_pos.y = j["Position"][1].get<float>();
    }

    auto readColor = [&](const char* key, ImVec4& target) {
        if (j.contains(key) && j[key].is_array() && j[key].size() == 4) {
            target.x = j[key][0].get<float>();
            target.y = j[key][1].get<float>();
            target.z = j[key][2].get<float>();
            target.w = j[key][3].get<float>();
        }
        };
  
    readColor("Color", uiMenu->color);
    readColor("Color_anglehelper", uiMenu->anglehelper_color);
    readColor("Color_90_lines", uiMenu->lines_color);

    uiMenu->copied_position = j.value("LastCopiedPosition", uiMenu->copied_position);
}

void Dorobot::saveConfiguration() 
{
    json j;

    j["Speedometer"] = uiMenu->velo_meter;
    j["SepVelo"] = uiMenu->sep_velo;
    j["PosHud"] = uiMenu->debugToggle;
    j["Anglehelper"] = uiMenu->anglehelper_toggle;
    j["90_Lines"] = uiMenu->lines_toggle;
    j["FPSWheel"] = uiMenu->fpswheel_toggle;
    j["FPSWheel_centerline"] = uiMenu->fpswheel_centerline_toggle;
    j["pfps"] = uiMenu->pfps_toggle;
    j["strafebot"] = uiMenu->strafebot_toggle;
    j["keyOnW_toggle"] = uiMenu->keyOnW_toggle;
    j["keyOnWWDSelected"] = uiMenu->keyOnWWDSelected;
    j["sprintToPrestrafe"] = uiMenu->sprintToPrestrafe_toggle;
    j["sprintToPrestrafeIsWD"] = uiMenu->sprintToPrestrafeIsWD;
    j["jumpAtVelo"] = uiMenu->jumpAtVelo_toggle;
    j["Scale"] = uiMenu->velo_scale;
    j["FPSWheelOffsetY"] = uiMenu->fpswheel_offset_y;
    j["FPSWheelOffsetX"] = uiMenu->fpswheel_offset_x;
    j["FPSWheelSize"] = uiMenu->fpswheel_size;
    j["Anglehelper_pixel_scale"] = uiMenu->ah_pixel_scale;
    j["Wheel_anglehelper_pixel_scale"] = uiMenu->wheel_ah_pixel_scale;
    j["Wheel_pixel_scale"] = uiMenu->wheel_pixel_scale;
    j["veloToJumpAt"] = uiMenu->veloToJumpAt;
    j["Position"] = { uiMenu->velo_pos.x, uiMenu->velo_pos.y };
    j["Color"] = { uiMenu->color.x, uiMenu->color.y, uiMenu->color.z, uiMenu->color.w };
    j["Color_anglehelper"] = { uiMenu->anglehelper_color.x, uiMenu->anglehelper_color.y, uiMenu->
anglehelper_color.z, uiMenu->anglehelper_color.w };
    j["Color_90_lines"] = { uiMenu->lines_color.x, uiMenu->lines_color.y, uiMenu->lines_color.z, uiMenu->lines_color.w };
    j["LastCopiedPosition"] = uiMenu->copied_position;
    j["autopara_toggle"] = uiMenu->autopara_toggle;
    j["auto250Toggle"] = uiMenu->auto250_toggle;
    j["autoparaOnlyOnce"] = uiMenu->autoparaOnlyOnce;
    j["autoTransferzone3Spam"] = uiMenu->autoTransferzone3Spam_toggle;
    j["autoTransferzone3SpamOnlyOnce"] = uiMenu->autoTransferzone3SpamOnlyOnce;
    j["rpgLookdown"] = uiMenu->rpgLookdown_toggle;
    j["rpgLookdownBackwards"] = uiMenu->rpgLookdownBackwards;
    j["rpgLookdownPitch"] = uiMenu->rpgLookdownPitch;
    j["rpgLookdownYaw"] = uiMenu->rpgLookdownYaw;
    j["jumpOnRpg"] = uiMenu->jumpOnRpg_toggle;
    j["displayAnglesToggle"] = uiMenu->displayAngles_toggle;
    j["bhopToggle"] = uiMenu->bhop_toggle;
    j["yawScriptToggle"] = uiMenu->yawScript_toggle;
    j["yawScriptAngle"] = uiMenu->yawScriptAngle;
    j["no200Toggle"] = uiMenu->no200_toggle;
	j["binds"] = bindManager->binds;
    std::ofstream out(CONFIG);
    out << j.dump(2) << std::endl;
}

Dorobot::Dorobot()
{
	exit = false;
	dorobot = this;
    setupFolders();
	hookWrapper = std::shared_ptr<HookWrapper>(new HookWrapper);
	commonHooks = std::shared_ptr<CommonHooks>(new CommonHooks(this));
	game = std::shared_ptr<Game>(new Game());
	input = std::shared_ptr<Input>(new Input(this));
	render = std::shared_ptr<Render>(new Render(this));
	
	uiSettings = std::shared_ptr<UI_Settings>(new UI_Settings(this));
	uiDebug = std::shared_ptr<UI_Debug>(new UI_Debug(this));
	uiVelocity = std::shared_ptr<UI_Velocity>(new UI_Velocity(this));
	uiView = std::shared_ptr<UI_View>(new UI_View(this));
	uiMenu = std::shared_ptr<UI_Menu>(new UI_Menu(this));
	ui90Lines = std::shared_ptr<UI_90Lines>(new UI_90Lines(this));
	uiFpsWheel = std::shared_ptr<UI_FpsWheel>(new UI_FpsWheel(this));

	prediction = std::shared_ptr<Prediction>(new Prediction(this));
	strafeBot = std::shared_ptr<StrafeBot>(new StrafeBot(this));
	automatition = std::shared_ptr<Automatition>(new Automatition(this));
	textureManager = std::shared_ptr<TextureManager>(new TextureManager(this));
	recorder = std::shared_ptr<Recorder>(new Recorder(this));
	positioning = std::shared_ptr<Positioning>(new Positioning(this));
	bindManager = std::shared_ptr<BindManager>(new BindManager(this));
	sessionManager = std::shared_ptr<SessionManager>(new SessionManager(this));
    strafeBot->registerBinds();
    automatition->registerBinds();
    recorder->registerBinds();

	//Added both INSERT and F6 to open the menu for people who have smaller keyboards and cant find that INSERT key ¬_¬
	input->addCallback(VK_INSERT, [this](UINT key_state) { return this->bindToggleInput(key_state); });
	input->addCallback(VK_CAPITAL, [this](UINT key_state) { return this->bindToggleInput(key_state); });

	//Keybind to tp to the last saved postion
	input->addCallback(VK_F3, [this](UINT key_state) { return this->bindTpToSavedPos(key_state); });

	loadConfiguration();
}

Dorobot::~Dorobot()
{
}

Dorobot* Dorobot::getInstance()
{
	return dorobot;
}

void Dorobot::setupFolders()
{
    namespace fs = std::filesystem;
    if (!fs::is_directory("Dorobot") || !fs::exists("Dorobot")) {
        fs::create_directory("Dorobot");
    }
    if (!fs::is_directory(Recorder::recordFolderPath) || !fs::exists(Recorder::recordFolderPath)) {
        fs::create_directory(Recorder::recordFolderPath);
    }
}
