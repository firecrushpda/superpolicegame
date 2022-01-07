//=============================================================================
//
// �G���W������ [main.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "WindowContainer.h"
#include "Timer.h"
#include "../DirectX 11 Engine VS2017/Graphics/Collision.h"
#include <atlstr.h>

class Engine : WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
	void ChangeStats(GameState state);

private:

	Timer timer;//�^�C�}�[

	Timer chasecarcdtimer;

	//DirectX::ContainmentType precoreslut;
	
	bool testbool = false;

	bool mIsInput;
	
	Camera3D tempcam;

};