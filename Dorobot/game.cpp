#include "pch.h"
#include "Game.h"

HWND Game::getWindow()
{
	return *(HWND*)0xCC1B6FC;
}
bool Game::isFocused()
{
	return *(bool*)0x0CC1B704;
}

bool Game::isInMainMenu()
{
	return *reinterpret_cast<int*>(addr_ingame) == connection_state_uninitialized;
}

LPDIRECT3DDEVICE9 Game::get_device()
{
	return *(LPDIRECT3DDEVICE9*)0xCC9A408;
}

bool Game::isConnected()
{
	int state = *(int*)addr_ingame;
	return state == connection_state_connected;
}

Vec3<float> Game::getView()
{
	Vec3<float> deltaAngles = *reinterpret_cast<Vec3<float>*>(addr_deltaAngles);
	Vec3<float> cameraAngles = *reinterpret_cast<Vec3<float>*>(addr_view_writable);
	Vec3<float> normalized;

	normalized.x = mm::normalise(cameraAngles.x + deltaAngles.x, 0.f, 360.f);
	normalized.y = mm::normalise(cameraAngles.y + deltaAngles.y, 0.f, 360.f);
	normalized.z = mm::normalise(cameraAngles.z + deltaAngles.z, 0.f, 360.f);

	return normalized;
}

Vec3<float> Game::getOrigin()
{
	playerState_s* ps = (playerState_s*)(addr_playerState);
	return ps->origin;
}

Vec3<float> Game::getVelocity()
{
	Vec3<float> vel = *(Vec3<float>*)addr_velocity;
	return vel;
}

float Game::getDirDiff(bool invert)
{
	Lmove lMove = getLmove();
	if (invert) {
		lMove = invertLmove(lMove);
	}

	if (lMove.isForward) {
		return (lMove.isRight) ? -45.f : 45.f;
	}
	else {
		return (lMove.isRight) ? -90.f : +90.f;
	}

	return 0;
}

void Game::setFps(int fps)
{
	*reinterpret_cast<int*>(addr_maxfps) = fps;
}

Lmove Game::getLmove()
{
	using namespace mm;
	input_s* input = (input_s*)addr_usercmd;
	usercmd_s* cmd = input->GetUserCmd(input->currentCmdNum);
	Lmove lMove;

	lMove.isInAir = *reinterpret_cast<int*>(addr_inair) == 1023 ? true : false;
	lMove.isSprint = *reinterpret_cast<int*>(addr_sprint) >= 20 || *reinterpret_cast<int*>(addr_sprint) == 5 ? true : false;
	lMove.isMoving = fabsf(getVelocity().Length2D()) > 0 ? true : false;
	lMove.fullLean = *reinterpret_cast<float*>(addr_lean) >= 0.5f || *reinterpret_cast<float*>(addr_lean) >= 0.25f ? true : false;

	if (cmd->forward != 0)
	{
		lMove.isBack = cmd->forward == USERCMD_FORWARD_BACK ? true : false;
		lMove.isForward = cmd->forward == USERCMD_FORWARD_FORWARD ? true : false;
	}
	else
	{
		lMove.isBack = false;
		lMove.isForward = false;
	}

	if (cmd->side != 0)
	{
		lMove.isRight = cmd->side == USERCMD_SIDE_RIGHT ? true : false;
		lMove.isLeft = cmd->side == USERCMD_SIDE_LEFT ? true : false;
	}
	else
	{
		lMove.isLeft = false;
		lMove.isRight = false;
	}

	return lMove;
}

float Game::getVelocityAngle()
{
	using namespace mm;
	Vec3<float> velocity = *reinterpret_cast<Vec3<float>*>(addr_velocity);
	return normalise(tilt_angle(truncate_vector(getVelocity())), 0.f, 360.f);
}

int Game::callBG_FindWeaponIndexForName(char const* name)
{
	return reinterpret_cast<int(__cdecl*)(char const*)>(addr_BGFindWeaponIndexForName)(name);
}

bool Game::isDevmap()
{
	return *reinterpret_cast<int*>(0x01288600);
}


float Game::getDelta(bool invert)
{
	float accelAngle = 0.f;
	accelAngle = mm::normalise(getView().y + getDirDiff(invert), 0.f, 360.f);

	float delta = getVelocityAngle() - accelAngle;
	return mm::normalise(delta, -180.f, 180.f);
}

float Game::getDeltaOptimal(bool invert)
{	
	Lmove lmove = getLmove();
	if (invert) {
		lmove = invertLmove(lmove);
	}

	float speed = getVelocity().Length2D();
	float deltaOpt = mm::to_degrees(acosf((DEFAULT_GSPEED - getAccel(DEFAULT_GSPEED)) / speed));

	if (lmove.isLeft) {
		deltaOpt *= -1.f;
	}

	return deltaOpt;
}

float Game::getDeltaOptimal(float g_speed, bool invert)
{
	Lmove lmove = getLmove();;
	if (invert) {
		lmove = invertLmove(lmove);
	}

	float speed = getVelocity().Length2D();
	float deltaOpt = mm::to_degrees(acosf((g_speed - getAccel(g_speed)) / speed));

	if (lmove.isLeft) {
		deltaOpt *= -1.f;
	}

	return deltaOpt;
}

int Game::get_fps()
{
	int maxFps = *reinterpret_cast<int*>(addr_maxfps);
	if (maxFps == 0) maxFps = 333;
	return maxFps;
}

Lmove Game::invertLmove(const Lmove& lmove)
{
	Lmove invLmove = lmove;
	if (invLmove.isRight) {
		invLmove.isLeft = true;
		invLmove.isRight = false;
	}
	else {
		invLmove.isLeft = false;
		invLmove.isRight = true;
	}

	return invLmove;
}

float Game::getAccel(float gSpeed)
{
	return gSpeed / get_fps();
}

float Game::getOptimalAngle(bool invert)
{
	float delta = getDelta(invert);
	float deltaOptimal = getDeltaOptimal(invert);

	float yaw = getView().y;

	yaw += delta - deltaOptimal;
	return mm::normalise(yaw, 0.f, 360.f);
}


float Game::getOptimalAngle(float g_speed, bool invert)
{
	float delta = getDelta(invert);
	float deltaOptimal = getDeltaOptimal(g_speed, invert);
	if (isnan(deltaOptimal)) {
		deltaOptimal = 0;
		delta = 0;
	}

	float yaw = getView().y;

	yaw += delta - deltaOptimal;
	return mm::normalise(yaw, 0.f, 360.f);
}

bool Game::isOnGround()
{
	auto ps = getPmoveCurrent()->ps;
	if (ps) {
		return ps->GroundEntityNum != 1023;
	}
	return false;
}

int Game::getJumpTime()
{
	auto ps = (playerState_s*)(0x794474);

	return ps->jumpTime;
}

cg_t* Game::getCg_t()
{
	return reinterpret_cast<cg_t*>(0x0074E338);
}

Vec2<float> Game::getScreenRes()
{
	cg_t* ref = getCg_t();
	Vec2<float> screenres{};
	screenres.x = ref->Refdef.ScreenWidth;
	screenres.y = ref->Refdef.ScreenHeight;
	return screenres;
}

Vec3<float> Game::getDeltaAngles()
{
	return *reinterpret_cast<Vec3<float>*>(addr_delta_angles);
}

pmove_t* Game::getPmoveCurrent()
{
	return reinterpret_cast<pmove_t*>(addr_pMove_current);
}

void Game::setView(const Vec3<float>& angles)
{
	Vec3<float>* view = reinterpret_cast<Vec3<float>*>(addr_view_writable);
	*view = angles;
}

input_s* Game::getInput_s()
{
	input_s* input = (input_s*)addr_input_s;
	return input;
}

void Game::setNormalView(const Vec3<float>& angles)
{
	Vec3<float> codAngles = Dorobot::getInstance()->game->toCodAngles(angles);

	Vec3<float>* view = reinterpret_cast<Vec3<float>*>(addr_view_writable);
	*view = codAngles;
}

void Game::setPitch(float pitch)
{
	Vec3<float> codAngles = Dorobot::getInstance()->game->toCodAngles(Vec3<float>(pitch, 0, 0));

	Vec3<float>* view = reinterpret_cast<Vec3<float>*>(addr_view_writable);
	view->x = codAngles.x;
}

void Game::setYaw(float yaw)
{
	Vec3<float> codAngles = Dorobot::getInstance()->game->toCodAngles(Vec3<float>(0, yaw, 0));

	Vec3<float>* view = reinterpret_cast<Vec3<float>*>(addr_view_writable);
	view->y = codAngles.y;
}

Vec3<float> Game::toCodAngles(const Vec3<float>& angles)
{
	Vec3<float> delta = Dorobot::getInstance()->game->getDeltaAngles();
	Vec3<float> view;
	view.x = angles.x - delta.x;
	view.y = angles.y - delta.y;
	view.z = angles.z - delta.z;

	return view;
}

float Game::getFov()
{
	cvar_t* fov = getCvar("cg_fov");
	cvar_t* fovScale = getCvar("cg_fovScale");
	return fov->value.valueFloat * fovScale->value.valueFloat;
}

cvar_t* Game::getCvar(const char* name)
{
	DWORD addr = 0x56b5d0;
	__asm
	{
		mov edi, name
		call[addr]
	}
}

void Game::sendCommandToConsole(const char* command)
{
	DWORD buffer_cmd = 0x4f8d90;
	__asm
	{
		mov eax, command
		mov ecx, 0
		call buffer_cmd
	}
}

bool Game::worldToScreen(Vec3<float> world, float* screen_x, float* screen_y)
{
	cg_t* ref = getCg_t();
	
	Vec3<float> position = world - ref->Refdef.Origin;
	
	Vec3<float> transform;
	transform.x = position.DotProduct(ref->Refdef.ViewAxis[1]);
	transform.y = position.DotProduct(ref->Refdef.ViewAxis[2]);
	transform.z = position.DotProduct(ref->Refdef.ViewAxis[0]);
	
	if (transform.z < 0.1f)
		return false;

	Vec2 center = { ref->Refdef.ScreenWidth * 0.5f, ref->Refdef.ScreenHeight * 0.5f };

	*screen_x = center.x * (1 - (transform.x / ref->Refdef.FOV.x / transform.z));
	*screen_y = center.y * (1 - (transform.y / ref->Refdef.FOV.y / transform.z));

	return true;
}

int Game::getFpsWtmod()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_wtmod);
	return maxFps;
}

int Game::getFps3xp()
{
	int maxFps = (int)*reinterpret_cast<float*>(addr_maxfps_3xp);
	return maxFps;
}

void Game::addObituary(const std::string& msg)
{
	std::string final_msg = msg + "\n";
	reinterpret_cast<void(__cdecl*)(conChannel_t, const char*, msgtype_t)>(0x4FCA50)(conChannel_t::CON_CHANNEL_GAMENOTIFY, final_msg.c_str(), msgtype_t::MSG_DEFAULT);
}

bool Game::isNocliping()
{
	pmove_t* pm = getPmoveCurrent();
	bool nocliping = false;
	if (pm->ps) {
		nocliping = pm->ps->pm_type & PM_NOCLIP;
	}
	return nocliping;
}

bool Game::isSpectating()
{
	pmove_t* pm = getPmoveCurrent();
	bool nocliping = false;
	if (pm->ps) {
		nocliping = pm->ps->pm_type & PM_SPEC;
	}
	return nocliping;
}

bool Game::isSprinting()
{
	pmove_t* pm = getPmoveCurrent();
	bool sprinting = false;
	if (pm->ps) {
		sprinting = pm->ps->pm_flags & PMF_SPRINTING;
	}
	return sprinting;
}

std::string Game::getPlayerName()
{
	return std::string(getCvar("name")->value.valueString);
}

std::string Game::getMapName()
{
	return std::string(getCvar("mapname")->value.valueString);
}
