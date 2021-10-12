//=============================================================================
//
//�L�[�{�[�h���� [KeyboardEvent.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once

class KeyboardEvent
{
public:
	//�L�[�{�[�h�������
	enum EventType
	{
		Press,//����
		Release,//���
		Invalid//�֎~
	};

	KeyboardEvent();
	KeyboardEvent(const EventType type, const unsigned char key);
	bool IsPress() const;
	bool IsRelease() const;
	bool IsValid() const;
	unsigned char GetKeyCode() const;

private:
	EventType type;//�L�[�{�[�h�������
	unsigned char key;//�L�[
};