//=============================================================================
//
// レンダオブジェクト処理 [RenderableGameObject.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "RenderableGameObject.h"

//=============================================================================
// 初期化
//=============================================================================
bool RenderableGameObject::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;

	if (ProcessCollsion())
		return false;
	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->UpdateMatrix();
	return true;
}

//=============================================================================
//描画
//=============================================================================
void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// マトリクス更新処理
//=============================================================================
void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// 更新処理
//=============================================================================
void RenderableGameObject::Update(float dt, const XMMATRIX & viewProjectionMatrix)
{
	model.Update(dt, this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// 骨のフラッグ
//=============================================================================
void RenderableGameObject::SetSkeletonDebugFlag() 
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// アニメーション再生
//=============================================================================
void RenderableGameObject::PlayAnimation(unsigned int num,AnimationPlayStyle aps) 
{
	model.PlayAnimaition(num, aps);
}

//=============================================================================
// 現在再生するアニメーション
//=============================================================================
unsigned int RenderableGameObject::GetCurrentAnimationClipIndex()
{
	return model.GetCurrentAnimationClipIndex();
}

//=============================================================================
// アニメーション数
//=============================================================================
unsigned int RenderableGameObject::GetAnimationClipSize()
{
	return model.GetAnimationClipSize();
}

//=============================================================================
// アニメーション再生スピード設置
//=============================================================================
void RenderableGameObject::SetAnimationSpeed(float speed)
{
	model.SetAnimationSpeed(speed);
}

//=============================================================================
// アニメーション再生スピード取る
//=============================================================================
float RenderableGameObject::GetAnimationSpeed() {
	return model.GetAnimationSpeed();
}

//=============================================================================
// 骨を見るか
//=============================================================================
void RenderableGameObject::ShowBones()
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// メッシュ情報
//=============================================================================
std::vector<Mesh> RenderableGameObject::GetMesh() {
	return this->model.GetMesh();
}

//=============================================================================
// ワールドマトリックスを取る
//=============================================================================
DirectX::XMMATRIX RenderableGameObject::GetWorldMatirx() {
	return  this->worldMatrix;
}

//=============================================================================
// 調整マトリックスを設置
//=============================================================================
void RenderableGameObject::SetGlobalMatirx(DirectX::XMMATRIX worldmat)
{
	this->model.m_GlobalInverseTransform = worldmat;
}

//=============================================================================
//当たり判定ブロック
//=============================================================================
void RenderableGameObject::ProcessCollsion(aiNode * node, const aiScene * pmScene,
	CollsionType cotype, bool showflag,DirectX::XMMATRIX oritrans) {
	//body collision
	collision = new CollsionObject();
	collision->boneindex = 1;//pelvis index
	collision->ct = CollsionType::Player;
	collision->debugmeshflag = false;
	auto cnode = node->mChildren[0];
	aiMesh* mesh = pmScene->mMeshes[cnode->mMeshes[0]];
	collision->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	collision->aabb = BoundingBox();
	collision->oritransform = DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f);
}


//=============================================================================
//デイバッグメッシュを読み込む
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh RenderableGameObject::ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.bone_index.w = -1;
		vertex.bone_index.x = -1;
		vertex.bone_index.y = -1;
		vertex.bone_index.z = -1;

		vertex.bone_weights.w = -1;
		vertex.bone_weights.x = -1;
		vertex.bone_weights.y = -1;
		vertex.bone_weights.z = -1;

		vertices.push_back(vertex);

	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			int index = face.mIndices[j];
			indices.push_back(face.mIndices[j]);
		}
	}

	std::vector<Texture> textures;
	aiMaterial* material = pmScene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, pmScene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());

	std::vector<Texture> specularTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_SPECULAR, pmScene);
	textures.insert(textures.end(), specularTextures.begin(), specularTextures.end());

	std::vector<Texture> normalTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_HEIGHT, pmScene);
	textures.insert(textures.end(), normalTextures.begin(), normalTextures.end());

	std::vector<Texture> depthTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DISPLACEMENT, pmScene);
	textures.insert(textures.end(), depthTextures.begin(), depthTextures.end());

	return Mesh( this->device, this->deviceContext, vertices, indices, textures, transformMatrix, mesh->mName.data);
}