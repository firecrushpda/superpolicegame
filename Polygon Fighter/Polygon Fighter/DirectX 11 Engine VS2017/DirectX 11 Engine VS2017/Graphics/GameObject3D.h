//=============================================================================
//
// Q[IuWFNg2DNX [GameObject2D.h]
// Author : GP11B183 28 āN B― (p@C^)
//
//=============================================================================
#pragma once
#include "GameObject.h"

class GameObject3D : public GameObject
{
public:

	void SetLookAtPos(DirectX::XMFLOAT3 lookAtPos);
	const DirectX::XMVECTOR & GetForwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetRightVector(bool omitY = false);
	const DirectX::XMVECTOR & GetBackwardVector(bool omitY = false);
	const DirectX::XMVECTOR & GetLeftVector(bool omitY = false);

protected:
	virtual void UpdateMatrix();

	void UpdateDirectionVectors();

	//fBtHg
	const DirectX::XMVECTOR DEFAULT_FORWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_UP_VECTOR = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_BACKWARD_VECTOR = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_LEFT_VECTOR = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR DEFAULT_RIGHT_VECTOR = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR vec_forward;//OüŦ
	DirectX::XMVECTOR vec_left;//ķÓ
	DirectX::XMVECTOR vec_right;//EÓ
	DirectX::XMVECTOR vec_backward;//ãë

	DirectX::XMVECTOR vec_forward_noY;//OüŦYēēŦ
	DirectX::XMVECTOR vec_left_noY;//ķÓYēēŦ
	DirectX::XMVECTOR vec_right_noY;//EÓYēēŦ
	DirectX::XMVECTOR vec_backward_noY;//ãëYēēŦ
};