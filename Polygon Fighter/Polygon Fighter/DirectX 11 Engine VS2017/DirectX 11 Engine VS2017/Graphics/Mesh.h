//=============================================================================
//
// ���b�V���N���X[Mesh.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, 
		std::vector<Vertex3D> & vertices, std::vector<DWORD> & indices, 
		std::vector<Texture> & textures, 
		const DirectX::XMMATRIX & transformMatrix, 
		std::string name);
	Mesh(const Mesh & mesh);
	std::vector<Vertex3D> vertices;//�o�[�e�b�N�X���
	std::vector<DWORD> indices;//�C���f�b�N�X���
	DirectX::XMMATRIX transformMatrix;//�ʒu
	bool isActive = true;//�\���܂���

	void Draw();
	const DirectX::XMMATRIX & GetTransformMatrix();
	//DirectX::XMMATRIX BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms);
	void RefreshVerTexBuffer(ID3D11DeviceContext* devicecontext, std::vector<Vertex3D> varray, UINT vertexCount);

	ID3D11Buffer* GetVertexBuffer();//VertexBuffer<Vertex3D> 
	ID3D11Buffer* GetIndexBuffer();//IndexBuffer*
private:
	std::string name;//���O
	VertexBuffer<Vertex3D> vertexbuffer;//�o�[�e�b�N�X�o�b�t�@
	IndexBuffer indexbuffer;//�C���f�b�N�X�o�b�t�@
	ID3D11DeviceContext * deviceContext;
	std::vector<Texture> textures;//�e�N�X�`��
	
	std::vector<VertexBoneInfo> vertexBoneInfo;//�����A���[(���g�p
	
};