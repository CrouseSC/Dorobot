#include "pch.h"
#include "Render.h"
#include "Dorobot.h"

void init_graphics_stub()
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->render)
		doroBot->render->initGraphics();
}

void __cdecl EngineDraw_Hook()
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->hookWrapper && doroBot->render)
	{
		doroBot->hookWrapper->hookMap["EngineDraw"]->original(EngineDraw_Hook)();
		doroBot->render->engineDraw();
	}
}

HRESULT __stdcall EndScene_Hook(LPDIRECT3DDEVICE9 dev)
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->hookWrapper && doroBot->render)
	{
		auto orig = doroBot->hookWrapper->hookMap["EndScene"]->original(EndScene_Hook)(dev);
		doroBot->render->endScene(dev);
		return orig;
	}
	return 1;
}

HRESULT __stdcall Reset_Hook(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{

	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->hookWrapper && doroBot->render)
	{

		auto orig = doroBot->hookWrapper->hookMap["Reset"]->original(Reset_Hook);
		doroBot->render->invalidateObjects(pDevice);
		HRESULT rval = orig(pDevice, pPresentationParameters);
		doroBot->render->createObjects(pDevice);
		return rval;
	}
	return 1;
}

void imguiStyle2()
{
	// DUCK RED nope! is DARK RED style by for40255 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.05490196123719215f, 0.05490196123719215f, 0.05490196123719215f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1176470592617989f, 0.1176470592617989f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9411764740943909f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.5647059082984924f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.501960813999176f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.8154506683349609f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8156862854957581f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.501960813999176f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.7450980544090271f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.6566523313522339f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.8039215803146362f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.501960813999176f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.6695278882980347f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9570815563201904f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1019607856869698f, 0.1137254908680916f, 0.1294117718935013f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2039215713739395f, 0.2078431397676468f, 0.2156862765550613f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 0.2000000029802322f);
	style.Colors[ImGuiCol_Tab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4392156898975372f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 0.9725490212440491f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4274509847164154f, 0.3607843220233917f, 0.3607843220233917f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.7124463319778442f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 0.6274510025978088f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.2588235437870026f, 0.2705882489681244f, 0.3803921639919281f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1803921610116959f, 0.2274509817361832f, 0.2784313857555389f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void imguiStyle1()
{
	// Fork of Red Oni style from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6000000238418579f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 8.0f;
	style.WindowBorderSize = 0.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 8.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 6.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 10.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Left;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.08235294371843338f, 0.1647058874368668f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(1.0f, 0.0f, 0.4117647111415863f, 0.5f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.9399141669273376f, 0.0f, 0.582379162311554f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 0.0f, 0.6196078658103943f, 0.6866952776908875f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.1882352977991104f, 0.6700000166893005f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.6180257797241211f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.6196078658103943f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1333333402872086f, 0.1333333402872086f, 0.1333333402872086f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.0f, 0.1372549086809158f, 0.4901960790157318f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8156862854957581f, 0.1215686276555061f, 0.407843142747879f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.5137255191802979f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.6196078658103943f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.8588235378265381f, 0.2666666805744171f, 0.5098039507865906f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.5803921818733215f, 0.0f, 0.239215686917305f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.95686274766922f, 0.0f, 0.3921568691730499f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.2745098173618317f, 0.3411764800548553f, 0.4156862795352936f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8039215803146362f, 0.364705890417099f, 0.5450980663299561f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9019607901573181f, 0.1098039224743843f, 0.4313725531101227f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.8619999885559082f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1921568661928177f, 0.1921568661928177f, 0.1921568661928177f, 0.800000011920929f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8039215803146362f, 0.0f, 0.3294117748737335f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.1176470592617989f, 0.47843137383461f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}


void Render::SetupImGuiStyle2()
{	
	if (firstRunStyle)
	{
		imguiStyle2();
		firstRunStyle = false;
	}
}

void Render::loadTextures()
{
	doroBot->textureManager->loadTextureFromFile("dorostrafe", "Dorobot/Images/dorostrafe.png");
}

void Render::initImgui(LPDIRECT3DDEVICE9 dev)
{
	if (!imguiInitialized)
	{
		if (ImGui::GetCurrentContext())
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			firstRunStyle = true;
		}

		Dorobot* doroBot = Dorobot::getInstance();
		
		ImGui_ImplDX9_InvalidateDeviceObjects();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = false;
		ImGui_ImplWin32_Init(Dorobot::getInstance()->game->getWindow());
		ImGui_ImplDX9_Init(dev);

		ImFontConfig fontConfig;
		fontConfig.FontDataOwnedByAtlas = false;
		doroBot->toxicFont = io.Fonts->AddFontFromMemoryTTF((void*)_acbahnschrift, sizeof(_acbahnschrift) - 1, 24.f, &fontConfig);
		doroBot->sepFont = io.Fonts->AddFontFromMemoryTTF((void*)_acawesomefont1, sizeof(_acawesomefont1) - 1, 24.f, &fontConfig);

		ImGui_ImplDX9_CreateDeviceObjects();
		imguiInitialized = true;
		SetupImGuiStyle2();
	}
	dev->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFF);
}

void __cdecl Render::engineDraw()
{
	Dorobot* doroBot = Dorobot::getInstance();

	if(doroBot->game->isConnected() && doroBot->uiMenu->lines_toggle)
	{
		doroBot->ui90Lines->render();
	}
}

void Render::endScene(LPDIRECT3DDEVICE9 dev)
{
	initImgui(dev);
	auto& io = ImGui::GetIO();

	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot->wantInput)
		io.MouseDrawCursor = true;
	else
		io.MouseDrawCursor = false;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	for (auto& fn : callbacksRender)
		fn();

	if (doroBot->bindManager) {
		doroBot->bindManager->cycle();
	}
	if (doroBot->sessionManager) {
		doroBot->sessionManager->cycle();
	}
	if (doroBot->recorder) {
		doroBot->recorder->renderProgressbar();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Render::addCallback(RenderCallback render)
{
	callbacksRender.push_back(render);
}

void Render::invalidateObjects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}
void Render::createObjects(LPDIRECT3DDEVICE9 pDevice)
{
	ImGui_ImplDX9_CreateDeviceObjects();
}
void Render::initGraphics()
{
	Dorobot* doroBot = Dorobot::getInstance();
	//call the original function first
	doroBot->hookWrapper->hookMap["InitGraphics"]->original(init_graphics_stub)();
	static LPDIRECT3DDEVICE9 current_device = nullptr;
	if (current_device != doroBot->game->get_device())
	{
		if (current_device)
		{
			D3DPRESENT_PARAMETERS p;
			current_device->Reset(&p);
		}
		Dorobot* doroBot = Dorobot::getInstance();

		if (doroBot && doroBot->hookWrapper) //remove the old hooks
		{
			if (doroBot->hookWrapper->hookMap.find("EndScene") != doroBot->hookWrapper->hookMap.end())
				doroBot->hookWrapper->hookMap["EndScene"]->remove();
			if (doroBot->hookWrapper->hookMap.find("Reset") != doroBot->hookWrapper->hookMap.end())
				doroBot->hookWrapper->hookMap["Reset"]->remove();
			if (doroBot->hookWrapper->hookMap.find("EngineDraw") != doroBot->hookWrapper->hookMap.end())
				doroBot->hookWrapper->hookMap["EngineDraw"]->remove();
		}

		current_device = doroBot->game->get_device();
		uint32_t* g_methodsTable = (uint32_t*)::calloc(119, sizeof(uint32_t));
		if (g_methodsTable)
		{
			imguiInitialized = false;
			::memcpy(g_methodsTable, *(uint32_t**)(doroBot->game->get_device()), 119 * sizeof(uint32_t));
			doroBot->hookWrapper->Add("EndScene", g_methodsTable[42], EndScene_Hook, HOOK_TYPE_DETOUR);
			doroBot->hookWrapper->Add("Reset", g_methodsTable[16], Reset_Hook, HOOK_TYPE_DETOUR);
			mem::memSet(0x6496d8, 0x90, 3); //disable check for developer to engine draw
			doroBot->hookWrapper->Add("EngineDraw", addr_engine_draw, EngineDraw_Hook, HOOK_TYPE_DETOUR);
			//update the wndproc hook on init
			doroBot->input->updateWndproc(doroBot->game->getWindow());
		}
	}
	loadTextures();
}

Render::Render(Dorobot* doroBot)
{
	this->doroBot = doroBot;
	//doing it this way only works if its loaded before initgraphics is called
	doroBot->hookWrapper->Add("InitGraphics", 0x5f4f09, init_graphics_stub, HOOK_TYPE_REPLACE_CALL);
}

Render::~Render() //hooks are removed when the hook wrapper is destroyed
{
	Dorobot* doroBot = Dorobot::getInstance();
	if (doroBot && doroBot->hookWrapper)
	{
		if (doroBot->hookWrapper->hookMap.count("InitGraphics") > 0)
			doroBot->hookWrapper->hookMap["InitGraphics"]->remove(); //remove hook here in case of a race condition on destructors
		if (doroBot->hookWrapper->hookMap.count("EndScene") > 0)
			doroBot->hookWrapper->hookMap["EndScene"]->remove(); //remove hook here in case of a race condition on destructors
		if (doroBot->hookWrapper->hookMap.count("Reset") > 0)
			doroBot->hookWrapper->hookMap["Reset"]->remove(); //remove hook here in case of a race condition on destructors
		if (doroBot->hookWrapper->hookMap.count("EngineDraw") > 0)
			doroBot->hookWrapper->hookMap["EngineDraw"]->remove(); //remove hook here in case of a race condition on destructors

	}
	ImGui::DestroyContext();
}