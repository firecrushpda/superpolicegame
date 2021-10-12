//=============================================================================
//
// FBX SDK�ǂݍ��� [FBXLoader.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once

#pragma comment(lib, "libfbxsdk-mt.lib")
#pragma comment(lib, "zlib-mt.lib")
#pragma comment(lib, "libxml2-mt.lib")
#pragma comment(lib, "libfbxsdk.lib")
//#pragma comment(lib, "d3dcompiler.lib")

#include <DirectXMath.h>
#include <vector>
#include <fbxsdk.h>
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Vertex.h"

//���b�V���X�g���N�`���[
class MeshFBX {
public:
	MeshFBX() {};
	~MeshFBX() {};


	FbxNode* m_meshNode = NULL;
	FbxMesh *m_mesh = NULL;
	ID3D11Buffer *VerBuffer = NULL;
	ID3D11Buffer *IndBuffer = NULL;
	Vertex3D* vertices;
};

//FBX�N���X
class FBX_Model
{
	
private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader;

	FbxManager *m_fbxManager = nullptr;
	FbxScene *m_fbxScene = NULL;
	FbxNode* m_meshNode = NULL;
	FbxMesh *m_mesh = NULL;
	ID3D11Buffer *VerBuffer = NULL;
	ID3D11Buffer *IndBuffer = NULL;
	Vertex3D* vertices;
	std::vector<MeshFBX*> m_meshes;

	bool hasAnimation;
	int AnimStackNumber = 0;
	FbxTime FrameTime, timeCount, start, stop;

public:
	FBX_Model();
	~FBX_Model();

	// <�`��>
	void Draw(
		const DirectX::XMMATRIX& world,
		const DirectX::XMMATRIX& viewproj);

	// <���f���쐬>
	void Create(
		ID3D11Device* device,
		ID3D11DeviceContext* context,
		ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader,
		const char* fbxfile_path);

	// <�j��>
	void Destroy();

private:
	void FBX_Import(const char* fbxfile_path);
	void FBX_GetVertex();
	void FBX_SetVertexData(ID3D11Device* device);
};


