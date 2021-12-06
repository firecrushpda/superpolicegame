//=============================================================================
//
// レンダゲームオブジェクト [RenderableGameObject.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"

class RenderableGameObject : public GameObject3D
{
public:
	bool b_use = true;

	bool b_modelview = true;

	std::string path = "";

	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);

	void BatchDraw(const XMMATRIX & viewProjectionMatrix, bool firstsign);

	void Update(float dt, const XMMATRIX & viewProjectionMatrix);

	void SetSkeletonDebugFlag();

	void PlayAnimation(unsigned int num, AnimationPlayStyle aps);

	unsigned int GetCurrentAnimationClipIndex();

	unsigned int GetAnimationClipSize();

	void SetAnimationSpeed(float speed);

	float GetAnimationSpeed();

	std::vector<Mesh> GetMesh();

	void ShowBones();

	DirectX::XMMATRIX GetWorldMatirx();

	void SetGlobalMatirx(DirectX::XMMATRIX worldmat);

	CollsionObject* GetCollisionObject();

	void SetCollisionBoxView(bool view);

	void DeepCopy(const RenderableGameObject& go);

private:
	ID3D11Device * device;
	ID3D11DeviceContext * deviceContext;
	ConstantBuffer<CB_VS_vertexshader>* cb_vs_vertexshader;

	//collision
	CollsionObject* collision = nullptr;

	bool ProcessCollsion(CollsionType cotype, bool showflag, DirectX::XMMATRIX oritrans);
	void UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	Mesh ProcessDebugMesh(const XMFLOAT3* corners);

	//texuture
	std::string directory = "";
	std::vector<Texture> LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene);
	TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType);
	int GetTextureIndex(aiString * pStr);

protected:

	Model model;//モデル

	void UpdateMatrix() override;

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();//位置
};