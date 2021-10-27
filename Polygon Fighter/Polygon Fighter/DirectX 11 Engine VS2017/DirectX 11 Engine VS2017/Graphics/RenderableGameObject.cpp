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
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	if (!model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;

	//default
	if (!ProcessCollsion(CollsionType::Player,true,DirectX::XMMatrixIdentity()))
		return false;

	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetScale(1.0f, 1.0f, 1.0f);//位置

	this->UpdateMatrix();
	return true;
}

//=============================================================================
//描画
//=============================================================================
void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	model.Draw(this->worldMatrix, viewProjectionMatrix);

	//collision debug block
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	if (!(collision == nullptr))
	{
		if (collision->debugmeshflag)
		{
			for (size_t i = 0; i < collision->debugmesh.size(); i++)
			{
				this->cb_vs_vertexshader->data.wvpMatrix = collision->oritransform * GetWorldMatirx() * viewProjectionMatrix;
				this->cb_vs_vertexshader->data.worldMatrix = collision->oritransform * GetWorldMatirx();
				this->cb_vs_vertexshader->ApplyChanges();

				collision->debugmesh.at(i).Draw();
			}
		}
	}
}

//=============================================================================
// マトリクス更新処理
//=============================================================================
void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z)
						* XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z)
						* XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// 更新処理
//=============================================================================
void RenderableGameObject::Update(float dt, const XMMATRIX & viewProjectionMatrix)
{
	model.Update(dt, this->worldMatrix, viewProjectionMatrix);

	//update collision
	UpdateCollisionBox(this->worldMatrix, viewProjectionMatrix);
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
bool RenderableGameObject::ProcessCollsion(CollsionType cotype, bool showflag,DirectX::XMMATRIX oritrans) {
	Assimp::Importer importer;

	const aiScene* Scene = importer.ReadFile("Data\\Objects\\debugBlock.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_JoinIdenticalVertices
	);

	if (!Scene)
		return false;

	//body collision
	collision = new CollsionObject();
	collision->ct = cotype;
	collision->collisionuse = true;
	collision->debugmeshflag = false;
	auto cnode = Scene->mRootNode->mChildren[0];
	aiMesh* mesh = Scene->mMeshes[cnode->mMeshes[0]];
	collision->debugmesh.push_back(this->ProcessDebugMesh(mesh, Scene, DirectX::XMMatrixIdentity()));
	collision->aabb = BoundingBox();
	collision->oritransform = oritrans;

	return true;
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

//=============================================================================
//マテリアル情報処理
//=============================================================================
std::vector<Texture> RenderableGameObject::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		case aiTextureType_SPECULAR:
			materialTextures.push_back(Texture(this->device, Color(255, 255, 255), textureType));
			return materialTextures;

		case aiTextureType_HEIGHT:
			materialTextures.push_back(Texture(this->device, Color(128, 128, 255), textureType));
			return materialTextures;

		case aiTextureType_DISPLACEMENT:
			materialTextures.push_back(Texture(this->device, Color(0, 0, 0), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(this->device,
					reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
					pScene->mTextures[index]->mWidth,
					textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture * pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device,
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;

}

//=============================================================================
//テクスチャ情報処理
//=============================================================================
TextureStorageType RenderableGameObject::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

//=============================================================================
//テクスチャインデックス情報
//=============================================================================
int RenderableGameObject::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

//=============================================================================
//衝突更新処理関数
//=============================================================================
void RenderableGameObject::UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	for (size_t i = 0; i < collision->debugmesh.size(); i++)
	{
		collision->aabb.Center = XMFLOAT3(0, 0, 0);
		collision->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
		auto collisonWorldMatrix = collision->oritransform * worldMatrix * viewProjectionMatrix;
		collision->aabb.Transform(collision->aabb, collisonWorldMatrix);
	}
}

//=============================================================================
//衝突可視化関数
//=============================================================================
void RenderableGameObject::SetCollisionBoxView(bool view) 
{
	this->collision->debugmeshflag = view;
}

//=============================================================================
//衝突取る関数
//=============================================================================
CollsionObject* RenderableGameObject::GetCollisionObject() 
{
	return this->collision;
}
