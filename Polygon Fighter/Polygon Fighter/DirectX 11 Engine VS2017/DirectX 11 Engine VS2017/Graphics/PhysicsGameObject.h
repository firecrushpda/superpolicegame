//=============================================================================
//
// レンダゲームオブジェクト [RenderableGameObject.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"
#include "../PhyXBasic.h"

class PhyGameObject : public GameObject3D
{
public:
	bool b_use = true;

	bool b_modelview = true;

	std::string path = "";

	PhysXBasic* phyworld;
	//PxRigidActor* actor;

	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, 
		ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader, PhysXBasic* phyworld);

	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);

	void Update();

	std::vector<Mesh> GetMesh();

	DirectX::XMMATRIX GetWorldMatirx();

	void SetGlobalMatirx(DirectX::XMMATRIX worldmat);

	void DeepCopy(const PhyGameObject& go);

	void RefreshVertexPosition(const PxGeometry& geom, unsigned int index);

	static DirectX::XMMATRIX GetMatrixFromPxMatrix(PxMat44 mat);

private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader;

	//tranform
	XMMATRIX m_GlobalInverseTransform = XMMatrixIdentity();

	std::vector<Mesh> meshes;

	Mesh ProcessDebugMesh(const PxGeometry& geom);

	//texuture
	std::string directory = "";
	std::vector<Texture> LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString * pStr);

protected:

	void UpdateMatrix() override;

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();//位置
};