#pragma once
#include <windows.h>
#include "xaudio2.h"

class Sound
{
public:

	enum
	{
		SOUND_LABEL_BGM_BetterDays,	// BGM
		SOUND_LABEL_BGM_chase,
		SOUND_LABEL_BGM_taxi,
		SOUND_LABEL_SE_kyaa,
		SOUND_LABEL_SE_syoutotu,
		SOUND_LABEL_MAX,
	};

	//*****************************************************************************
	// �p�����[�^�\���̒�`
	//*****************************************************************************
	typedef struct
	{
		WCHAR *pFilename;	// �t�@�C����
		int nCntLoop;		// ���[�v�J�E���g
	} SOUNDPARAM;

	// �e���f�ނ̃p�����[�^
	SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
	{
		{ (WCHAR*)L"Data/BGM/BetterDays.wav", 255 },	// BGM0
		{ (WCHAR*)L"Data/BGM/chaseBGM.wav", 255 },	// BGM1
		{ (WCHAR*)L"Data/BGM/taxiBGM.wav", 255 },	// BGM0
		{ (WCHAR*)L"Data/SE/kyaa.wav", 0 },
		{ (WCHAR*)L"Data/SE/syoutotu.wav", 0 },
	};

	//*****************************************************************************
	// �v���g�^�C�v�錾
	//*****************************************************************************
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	bool InitSound(HWND hWnd);
	void Update();
	void UninitSound(void);
	void PlayIndexSound(int label);
	void StopSound(int label);
	void StopSound(void);
	void FadeSound(int label);
	void PlayBGM(int label);
	void PlaySE(int label);

	//*****************************************************************************
	// �O���[�o���ϐ�
	//*****************************************************************************
	IXAudio2 *g_pXAudio2 = NULL;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
	IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// �}�X�^�[�{�C�X
	IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// �\�[�X�{�C�X
	BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^
	DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^�T�C�Y

	bool b_infadestate = false;
	unsigned int curbgmindex = -1;
	unsigned int tgbgmindex = -1;

private:

};