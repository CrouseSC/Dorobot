#include "pch.h"
#include "elebot.h"

Elebot::Elebot(Dorobot* doroBot)
{
	this->doroBot = doroBot;
}

Elebot::~Elebot()
{
}

void Elebot::cycle()
{
	if (doroBot->bindManager->bindActive("Select elevator")) {
		Dorobot::getInstance()->uiDebug->addDebuginfo("TRACING", 0.f);
		TraceResults results = callCGTrace();
		Dorobot::getInstance()->uiDebug->addDebuginfo("NORMAL X: ", results.trace.normal[0]);
		Dorobot::getInstance()->uiDebug->addDebuginfo("NORMAL Y: ", results.trace.normal[1]);
		Dorobot::getInstance()->uiDebug->addDebuginfo("FRACTION: ", results.trace.fraction);
		auto origin = doroBot->game->getOrigin();
		Dorobot::getInstance()->uiDebug->addDebuginfo("POS", &origin);
		this->traceResults = results;
		if (results.trace.material) {
			Dorobot::getInstance()->uiDebug->addDebuginfo(results.trace.material, 0.f);
		}
	}

	elevate();
}

void Elebot::registerBinds()
{
	doroBot->bindManager->registerBindName("Elevate", BIND_TYPE_HOLD);
	doroBot->bindManager->registerBindName("Select elevator", BIND_TYPE_HOLD);
}

void Elebot::renderMarkers()
{
	auto pos = traceResults.hitposReal;
	if (pos == Vec3<float>(0, 0, 0)) {  //default marker position
		return;
	}

	Vec2<float> screen;
	constexpr float SCALE_FACTOR = 10.f;
	constexpr float MAX_DIST = 2000.f;
	if (doroBot->game->worldToScreen(pos, &screen.x, &screen.y)) {
		if (animSetup) {
			doroWalk.updateAnimation();
			AnimationFrame frame = doroWalk.getCurrentFrame();
			float dist = fabsf(doroBot->game->getOrigin().Dist(pos));
			float sizeScale;
			if (dist > MAX_DIST) {
				sizeScale = 0.f;
			}
			else {
				sizeScale = MAX_DIST / dist * SCALE_FACTOR;
			}

			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(ImVec2(doroBot->game->getScreenRes().x, doroBot->game->getScreenRes().y));
			
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImGui::Begin("doroWalk", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoMove);
			ImGui::SetCursorPos(ImVec2(screen.x - sizeScale / 2, screen.y - sizeScale / 2));
			ImGui::Image(frame.id, ImVec2(sizeScale, sizeScale), ImVec2(frame.uv.x, frame.uv.y), ImVec2(frame.uv.x + 0.2, frame.uv.y + 0.25));
			ImGui::SetCursorPos(cursorPos);
			ImGui::End();
		}
	}
}

void Elebot::elevate()
{
	Vec3<float> pos = traceResults.hitposAdjusted;

	Axis axis = traceResults.trace.normal[0] != 0.f ? AXIS_X : AXIS_Y;
	if (doroBot->bindManager->bindActive("Elevate")) {
		if ((axis == AXIS_X && pos.x == doroBot->game->getOrigin().x) || (axis == AXIS_Y && pos.y == doroBot->game->getOrigin().y)) {
			usercmd_s* cmd = doroBot->game->getInput_s()->GetUserCmd(doroBot->game->getInput_s()->currentCmdNum);
			cmd->buttons = 0;  //uncrouch
			doingEle = false;
		}
		else {
			doingEle = doroBot->positioning->moveToElePosition(Vec2<float>(pos.x, pos.y), axis);
		}
	}
	else {
		doingEle = false;
	}
}

__declspec(naked)
void callCGTraceInternal(trace_t* results, Vec3<float>* start, Vec3<float>* mins, Vec3<float>* maxs, Vec3<float>* end, int passEntityNum, int contentMask, bool locational, bool staticModels)
{
	_asm {
		push ebp
		mov ebp, esp
		sub esp, __LOCAL_SIZE
		push staticModels
		push locational
		push contentMask
		push passEntityNum
		push end
		push maxs
		push start
		mov edi, results
		mov eax, mins
	    call addr_cg_trace
		add esp, 0xC
		mov esp, ebp
		pop ebp
		ret
	}
}

void Elebot::setupAnimation()
{
	SpritesheetWrapper* spriteSheet = dynamic_cast<SpritesheetWrapper*>(doroBot->textureManager->getTexture("dorowalk"));
	doroWalk.setSpriteSheet(spriteSheet);
	animSetup = true;
}

TraceResults Elebot::callCGTrace()
{
	typedef void CG_Trace(trace_t* results, Vec3<float>* start, Vec3<float>* mins, Vec3<float>* maxs,
		Vec3<float>* end, int passEntityNum, int contentMask, bool locational, bool staticModels);

	// taken from kej's aelebot
	playerState_s* ps = (playerState_s*)(addr_playerState);
	trace_t results;
	refdef_t* refdef = &doroBot->game->getCg_t()->Refdef;
	Vec3<float> nullvec(0, 0, 0);
	int contentMask = 0x2806831;
	Vec3<float> start = refdef->Origin;
	float HITBOX_SIZE = 16.f;
	float TRACE_CORRECTION = 0.125f;
	
	Vec3<float> view = doroBot->game->getView();
	view.x = mm::normalise(view.x * -1.f, 0.f, 360.f);
	Vec3<float> end = start + mm::sphericalToCartesian(9999.f, view.y, view.x);

	callCGTraceInternal(&results, &start, &nullvec, &nullvec, &end, 0, contentMask, false, false);

	Vec3<float> hitposAdjusted = (start + (end - start) * results.fraction) + (Vec3<float>(results.normal) * (HITBOX_SIZE - 1.f - TRACE_CORRECTION));
	Vec3<float> hitposReal = (start + (end - start) * results.fraction) + (Vec3<float>(results.normal) * (1.f - TRACE_CORRECTION));
	TraceResults traceResults;
	traceResults.trace = results;
	traceResults.hitposAdjusted = hitposAdjusted;
	traceResults.hitposReal = hitposReal;

	doroBot->uiDebug->addDebuginfo("Hitpos", &hitposAdjusted);
	return traceResults;
}

