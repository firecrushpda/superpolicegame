//=============================================================================
//
// �G���W������ [main.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "WindowContainer.h"
#include "Timer.h"


class Engine : WindowContainer
{
public:
	bool Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height);
	bool ProcessMessages();
	void Update();
	void RenderFrame();
private:
	Timer timer;//�^�C�}�[

	DirectX::ContainmentType precoreslut;
};