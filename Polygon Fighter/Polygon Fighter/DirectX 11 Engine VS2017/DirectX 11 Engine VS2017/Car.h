#pragma once
#include "../DirectX 11 Engine VS2017/Graphics/RenderableGameObject.h"

class Car : public RenderableGameObject
{
public:
	//renderobject
	RenderableGameObject carrender;

	bool CarInitialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void Update(float delta_time, const XMMATRIX & viewProjectionMatrix);
	void Draw(const XMMATRIX & viewProjectionMatrix);

private:
	std::string directory = "";

	//D3DX refer
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;

	//collision
	CollsionObject* carco = nullptr;
	//DirectX::ContainmentType precoreslut;
	void ProcessCollsion(aiNode * node, const aiScene * pmScene);
	void UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	Mesh ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);
	void LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	//debug
	std::vector<Mesh> debugBlocks;//デバッグ箱

	std::vector<Texture> LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString * pStr);
};

