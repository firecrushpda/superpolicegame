//=============================================================================
//
// 3D�Q�[���I�u�W�F�N�g [GameObject3D.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#include "Light.h"

//=============================================================================
// ������
//=============================================================================
bool Light::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize("Data/Objects/light.fbx", device, deviceContext, cb_vs_vertexshader))//
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();
	return true;
}