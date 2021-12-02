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
	// パラメータ構造体定義
	//*****************************************************************************
	typedef struct
	{
		WCHAR *pFilename;	// ファイル名
		int nCntLoop;		// ループカウント
	} SOUNDPARAM;

	// 各音素材のパラメータ
	SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
	{
		{ (WCHAR*)L"Data/BGM/BetterDays.wav", 255 },	// BGM0
		{ (WCHAR*)L"Data/BGM/chaseBGM.wav", 255 },	// BGM1
		{ (WCHAR*)L"Data/BGM/taxiBGM.wav", 255 },	// BGM0
		{ (WCHAR*)L"Data/SE/kyaa.wav", 0 },
		{ (WCHAR*)L"Data/SE/syoutotu.wav", 0 },
	};

	//*****************************************************************************
	// プロトタイプ宣言
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
	// グローバル変数
	//*****************************************************************************
	IXAudio2 *g_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// マスターボイス
	IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
	BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
	DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

	bool b_infadestate = false;
	unsigned int curbgmindex = -1;
	unsigned int tgbgmindex = -1;

private:

};