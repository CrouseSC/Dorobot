#pragma once
#include "cod4_structs.h"
#include "d3dx9/d3dx9.h"
#include "vectors.h"
#include "game_math.h"
#include "Lmove.h"
#include "memory.h"

enum connection_state_ : int
{
    connection_state_uninitialized,
    connection_state_disconnected,
    connection_state_authorizing,
    connection_state_connecting,
    connection_state_challenging,
    connection_state_established,
    connection_state_loading,
    connection_state_primed,
    connection_state_active,
    connection_state_connected
};

typedef struct cvar_s
{
	char* name;
	char* string;
	char* resetString;		// cvar_restart will reset to this value
	char* latchedString;		// for CVAR_LATCH vars
	int			flags;
	bool	modified;			// set each time the cvar is changed
	int			modificationCount;	// incremented each time the cvar is changed
	union
	{
		float		valueFloat;				// atof( string )
		const char* valueString;
	} value;
	int			integer;			// atoi( string )
	struct cvar_s* next;
	struct cvar_s* hashNext;
} cvar_t;

class Game
{
public:
	struct GfxPointVertex
	{
		Vec3<float> location;
		char color[4];
		GfxPointVertex() : location(0, 0, 0), color{ 0, 0, 0, 0 } {};
		GfxPointVertex(Vec3<float> loc, ImColor col)
		{
			location = loc;
			color[0] = col.Value.z * 255;
			color[1] = col.Value.y * 255;
			color[2] = col.Value.x * 255;
			color[3] = col.Value.w * 255;
		}
		GfxPointVertex(Vec3<float> loc, char r, char g, char b, char a)
		{
			location = loc;
			color[0] = b;
			color[1] = g;
			color[2] = r;
			color[3] = a;
		}
	};
	
	
	HWND getWindow(); 
	bool isFocused();
	bool isInMainMenu();
	LPDIRECT3DDEVICE9 get_device();
    bool isConnected();
    Vec3<float> getView();
    Vec3<float> getOrigin();
    Vec3<float> getVelocity();
    float getOptimalAngle(bool invert = false);
    float getOptimalAngle(float g_speed, bool invert = false);
	bool isOnGround();
	static void sendCommandToConsole(const char* command);
	bool worldToScreen(Vec3<float> world, float* screen_x, float* screen_y);
	int getFpsWtmod();
	int getFps3xp();
	void addObituary(const std::string& msg);
	int getJumpTime();
    Vec2<float> getScreenRes();
	mem::function<void(int count, int width, GfxPointVertex* verts, bool depthTest)> polyline = 0x613040;
    Lmove getLmove();
	float getFov();
	cvar_t* getCvar(const char* name);
    Vec3<float> getDeltaAngles();
	Vec3<float> toCodAngles(const Vec3<float>& angles);
	void setNormalView(const Vec3<float>& angles);
	void setPitch(float pitch);
	void setYaw(float pitch);
	void setView(const Vec3<float>& angles);
	pmove_t* getPmoveCurrent();
	input_s* getInput_s();
	cg_t* getCg_t();
	bool isNocliping();
	bool isSpectating();
	bool isSprinting();
	void setFps(int fps);
    int get_fps();
	std::string getPlayerName();
	std::string getMapName();
    float getVelocityAngle();
	int callBG_FindWeaponIndexForName(char const* name);

private:
    float getDelta(bool invert = false);
    float getDeltaOptimal(bool invert = false);
    float getDeltaOptimal(float g_speed, bool invert = false);
	float getDirDiff(bool invert = false);
    float getAccel(float gSpeed);
	Lmove invertLmove(const Lmove& lmove);
    constexpr static float DEFAULT_GSPEED = 190.f;

};

