//=============================================================================
//
// �Q�[���I�u�W�F�N�g2D�N���X [GameObject2D.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "GameObject.h"

class GameObject3D : public GameObject
{
public:
	float hp = 100;

	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const DirectX::XMVECTOR & GetForwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetRightVector(bool omitY = false);
	const DirectX::XMVECTOR & GetBackwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetLeftVector(bool omitY = false);

protected:
	virtual void UpdateMatrix();

	void UpdateDirectionVectors();

	//�f�B�t�H���g
	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR vec_forward;//�O����
	DirectX::XMVECTOR vec_left;//����
	DirectX::XMVECTOR vec_right;//�E��
	DirectX::XMVECTOR vec_backward;//���

	DirectX::XMVECTOR vec_forward_noY;//�O����Y������
	DirectX::XMVECTOR vec_left_noY;//����Y������
	DirectX::XMVECTOR vec_right_noY;//�E��Y������
	DirectX::XMVECTOR vec_backward_noY;//���Y������
};