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

	Timer fpstimer;
	fpstimer.Start();
	// �E�B���h�E�̍쐬
	if (engine.Initialize(hInstance, "Title", "MyWindowClass", 800, 600))
	{
		
		// ���b�Z�[�W���[�v
		while (engine.ProcessMessages() == true)
		{
			if (fpstimer.GetMilisecondsElapsed() >= 1000.0f / 60.0f)
			{
				engine.Update();		// �X�V���� deltatime_mili
				engine.RenderFrame();	// �`�揈��

				fpstimer.Restart();
			}
		}

	}
	return 0;
}