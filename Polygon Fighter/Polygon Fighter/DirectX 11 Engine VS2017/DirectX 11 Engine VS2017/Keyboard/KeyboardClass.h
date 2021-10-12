//=============================================================================
//
//�L�[�{�[�h�N���X [KeyboardClass.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "KeyboardEvent.h"
#include <queue>

class KeyboardClass
{
public:
	KeyboardClass();
	bool KeyIsPressed(const unsigned char keycode);
	bool KeyIsTriggered(const unsigned char keycode);
	bool KeyBufferIsEmpty();
	bool CharBufferIsEmpty();
	KeyboardEvent ReadKey();
	unsigned char ReadChar();
	void OnKeyPressed(const unsigned char key);
	void OnKeyReleased(const unsigned char key);
	void OnChar(const unsigned char key);
	void EnableAutoRepeatKeys();
	void DisableAutoRepeatKeys();
	void EnableAutoRepeatChars();
	void DisableAutoRepeatChars();
	bool IsKeysAutoRepeat();
	bool IsCharsAutoRepeat();
private:
	bool autoRepeatKeys = false;//�����I�ɃL�[���J��Ԃ�
	bool autoRepeatChars = false;//�����I�ɓ��͂��J��Ԃ�
	bool keyStates[256];//�L�[���
	bool oldkeyStates[256];//�L�[���
	std::queue<KeyboardEvent> keyBuffer;//�L�[�o�b�t�@
	std::queue<unsigned char> charBuffer;//���̓o�b�t�@
};