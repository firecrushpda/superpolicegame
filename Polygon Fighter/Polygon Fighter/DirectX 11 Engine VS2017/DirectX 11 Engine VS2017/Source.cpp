//=============================================================================
//
// �\�[�X���� [main.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#include "Engine.h"

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to call CoInitialize.");
		return -1;
	}

	Engine engine;
	// �E�B���h�E�̍쐬
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600))
	{
		// ���b�Z�[�W���[�v
		while (engine.ProcessMessages() == true)
		{
			engine.Update();		// �X�V����
			engine.RenderFrame();	// �`�揈��
		}
	}
	return 0;
}