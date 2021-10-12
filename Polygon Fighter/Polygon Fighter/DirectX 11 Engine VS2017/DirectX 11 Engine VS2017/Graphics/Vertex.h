//=============================================================================
//
// �o�[�e�b�N�X�N���X [Vertex.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include <DirectXMath.h>

struct Vertex2D
{
	Vertex2D() {}
	Vertex2D(float x, float y, float z, float u, float v)
		: pos(x, y, z), texCoord(u, v) {}

	DirectX::XMFLOAT3 pos;//�ʒu
	DirectX::XMFLOAT2 texCoord;//���W
};

struct Vertex3D
{
	Vertex3D() {}
	Vertex3D(float x, float y, float z, float u, float v, float nx, float ny, float nz)
		: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

	DirectX::XMFLOAT3 pos;//�ʒu
	DirectX::XMFLOAT2 texCoord;//���W
	DirectX::XMFLOAT3 normal;//�m�[�}��
	DirectX::XMFLOAT3 tangent;//����
	DirectX::XMFLOAT4 bone_index;//���C���f�b�N�X
	DirectX::XMFLOAT4 bone_weights;//���d��

};

struct Vertex3D_SkyBox
{
	Vertex3D_SkyBox() {}
	Vertex3D_SkyBox(float x, float y, float z)
		: pos(x, y, z) {}

	DirectX::XMFLOAT3 pos;//�ʒu
	DirectX::XMFLOAT2 texCoord;//���W
};

//assimp bone info 
//���g�p
struct VertexBoneInfo
{
	VertexBoneInfo(){}
	unsigned int bone_index[4];
	float weights[4];
	void AddBoneData(float Weight, unsigned int id)
	{
		for (unsigned int i = 0; i < 4; i++) {
			if (weights[i] == 0.0) {
				bone_index[i] = id;
				weights[i] = Weight;
				return;
			}
		}
	}
	unsigned int next_space = 0;
};