#pragma once
#include "d3dx9/d3dx9.h"
#include <functional>
typedef std::function<void()> RenderCallback;

class Render
{
public:
	Render(class Dorobot* doroBot);
	~Render();
	void initGraphics();
	void endScene(LPDIRECT3DDEVICE9 pDevice);
	void __cdecl engineDraw();
	void invalidateObjects(LPDIRECT3DDEVICE9 pDevice);
	void createObjects(LPDIRECT3DDEVICE9 pDevice);
	void initImgui(LPDIRECT3DDEVICE9 dev);
	void addCallback(RenderCallback);
	void SetupImGuiStyle2();
	bool firstRunStyle = true;
	bool assetsInit = false;

private:
	Dorobot* doroBot;
	bool imguiInitialized = false;
	std::vector<RenderCallback> callbacksRender{};
	void loadTextures();

};

