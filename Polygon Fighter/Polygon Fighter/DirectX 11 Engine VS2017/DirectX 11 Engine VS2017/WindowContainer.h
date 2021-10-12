//=============================================================================
//
// �E�C���h�E�Y���� [WindowContainer.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "RenderWindow.h"
#include "Keyboard/KeyboardClass.h"
#include "Mouse/MouseClass.h"
#include "Graphics/Graphics.h"

class WindowContainer
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	RenderWindow render_window; //�����_�����O�E�C���h�E
	KeyboardClass keyboard;//�L�[�{�[�h
	MouseClass mouse;//�}�E�X
	Graphics gfx;//�O���t�B�b�N�X
private:
};