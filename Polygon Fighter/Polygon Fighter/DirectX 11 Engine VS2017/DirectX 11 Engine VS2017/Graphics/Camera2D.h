//=============================================================================
//
// 2D�J�������� [Camera2D.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "GameObject2D.h"

class Camera2D : public GameObject2D
{
public:
	Camera2D();
	void SetProjectionValues(float width, float height, float nearZ, float farZ);

	const XMMATRIX & GetOrthoMatrix() const;//�I���\�J����
	const XMMATRIX & GetWorldMatrix() const;
private:
	void UpdateMatrix() override;

	XMMATRIX orthoMatrix;//�I���\�}�g���N�X
	XMMATRIX worldMatrix;
};