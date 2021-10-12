//=============================================================================
//
// �Q�[���I�u�W�F�N�g�N���X [GameObject.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "Model.h"

class GameObject
{
public:
	//�ʒu�Q�[�g
	const DirectX::XMVECTOR & GetPositionVector() const;
	const DirectX::XMFLOAT3 & GetPositionFloat3() const;
	const DirectX::XMVECTOR & GetRotationVector() const;
	const DirectX::XMFLOAT3 & GetRotationFloat3() const;

	//�ʒu�Z�b�g
	void SetPosition(const DirectX::XMVECTOR & pos);
	void SetPosition(const DirectX::XMFLOAT3 & pos);
	void SetPosition(float x, float y, float z);
	void AdjustPosition(const DirectX::XMVECTOR & pos);
	void AdjustPosition(const DirectX::XMFLOAT3 & pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR & rot);
	void SetRotation(const DirectX::XMFLOAT3 & rot);
	void SetRotation(float x, float y, float z);
	void AdjustRotation(const DirectX::XMVECTOR & rot);
	void AdjustRotation(const DirectX::XMFLOAT3 & rot);
	void AdjustRotation(float x, float y, float z);
	void SetScale(float xScale, float yScale, float zScale = 1.0f);

protected:
	virtual void UpdateMatrix();

	DirectX::XMVECTOR posVector;//�v�Z�p�|�W�V����
	DirectX::XMVECTOR rotVector;//�v�Z�p���[�e�[�V����
	DirectX::XMFLOAT3 pos;//�|�W�V����
	DirectX::XMFLOAT3 rot;//���[�e�[�V����

	DirectX::XMFLOAT3 scale;//�X�P�[��

	
};