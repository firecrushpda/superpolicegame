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

	this->SetRotation(0.0f, 0.0f, 0.0f);//回転
	this->SetPosition(0.0f, 0.0f, 0.0f);//位置
	this->SetScale(1.0f, 1.0f, 1.0f);//位置

	this->UpdateMatrix();

	//default
	if (!ProcessCollsion(CollsionType::Player, true, DirectX::XMMatrixIdentity()))
		return false;

	return true;
}

//=============================================================================
//描画
//=============================================================================
void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	if (!b_use) return;

	if (b_modelview)
	{
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		model.Draw(this->worldMatrix, viewProjectionMatrix);
	}

	//collision debug block
	deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	if (!(collision == nullptr))
	{
		if (collision->debugmeshflag)
		{
			for (size_t i = 0; i < collision->debugmesh.size(); i++)
			{
				/*auto matrixmesh = XMMatrixScaling(this->collision->obb.Extents.x, this->collision->obb.Extents.y, this->collision->obb.Extents.z)
					* XMMatrixRotationQuaternion(XMLoadFloat4(&this->collision->obb.Orientation))
					* XMMatrixTranslation(this->collision->obb.Center.x, this->collision->obb.Center.y, this->collision->obb.Center.z);*/
				this->cb_vs_vertexshader->data.wvpMatrix = model.m_GlobalInverseTransform  * GetWorldMatirx() * viewProjectionMatrix;//offsetmat
				this->cb_vs_vertexshader->data.worldMatrix = model.m_GlobalInverseTransform  * GetWorldMatirx();//offsetmat
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
void RenderableGameObject::PlayAnimation(unsigned int num, AnimationPlayStyle aps)
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
bool RenderableGameObject::ProcessCollsion(CollsionType cotype, bool showflag, DirectX::XMMATRIX oritrans) {

	//body collision
	collision = new CollsionObject();
	collision->ct = cotype;
	collision->collisionuse = true;
	collision->debugmeshflag = true;

	std::vector<XMFLOAT3> poss;
	auto meshes = model.GetMesh();
	for (size_t j = 0; j < meshes.size(); j++)
	{
		for (size_t i = 0; i < meshes.at(j).vertices.size(); i++)
		{
			poss.push_back(meshes.at(j).vertices.at(i).pos);
		}
	}
	//BoundingOrientedBox::CreateFromPoints(collision->obb, poss.size(), &poss.at(0), sizeof(XMFLOAT3));
	BoundingBox::CreateFromPoints(collision->obb, poss.size(), &poss.at(0), sizeof(XMFLOAT3));

	collision->collisionoriginextents = XMFLOAT3(collision->obb.Extents.x, collision->obb.Extents.y, collision->obb.Extents.z);
	collision->collisionoffsetpos = XMFLOAT3(collision->obb.Center.x - pos.x, collision->obb.Center.y - pos.y, collision->obb.Center.z - pos.z);
	collision->oritransform = XMMatrixScaling(this->collision->obb.Extents.x, this->collision->obb.Extents.y, this->collision->obb.Extents.z)
							//* XMMatrixRotationQuaternion(XMLoadFloat4(&this->collision->obb.Orientation))
							* XMMatrixTranslation(this->collision->obb.Center.x, this->collision->obb.Center.y, this->collision->obb.Center.z);
	collision->originobb = collision->obb;

	XMFLOAT3 corners[BoundingOrientedBox::CORNER_COUNT];
	collision->obb.GetCorners(corners);
	collision->debugmesh.push_back(this->ProcessDebugMesh(corners));

	return true;
}


//=============================================================================
//デイバッグメッシュを読み込む
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh RenderableGameObject::ProcessDebugMesh(const XMFLOAT3* corners)
{
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < 24; i++)
	{
		Vertex3D vertex;

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

	vertices.at(0).pos = corners[5];
	vertices.at(1).pos = corners[6];
	vertices.at(3).pos = corners[1];
	vertices.at(2).pos = corners[2];

	vertices.at(4).pos = corners[0];
	vertices.at(5).pos = corners[3];
	vertices.at(7).pos = corners[4];
	vertices.at(6).pos = corners[7];

	vertices.at(8).pos = corners[7];
	vertices.at(9).pos = corners[3];
	vertices.at(11).pos = corners[6];
	vertices.at(10).pos = corners[2];

	vertices.at(12).pos = corners[5];
	vertices.at(13).pos = corners[1];
	vertices.at(15).pos = corners[4];
	vertices.at(14).pos = corners[0];

	vertices.at(16).pos = corners[1];
	vertices.at(17).pos = corners[2];
	vertices.at(19).pos = corners[0];
	vertices.at(18).pos = corners[3];

	vertices.at(20).pos = corners[4];
	vertices.at(21).pos = corners[7];
	vertices.at(23).pos = corners[5];
	vertices.at(22).pos = corners[6];

	for (UINT i = 0; i < 4; ++i)
	{
		vertices.at(i).normal = XMFLOAT3(1, 0, 0);
		vertices.at(i + 4).normal = XMFLOAT3(-1, 0, 0);
		vertices.at(i + 8).normal = XMFLOAT3(0, 1, 0);
		vertices.at(i + 12).normal = XMFLOAT3(0, -1, 0);
		vertices.at(i + 16).normal = XMFLOAT3(0, 0, 1);
		vertices.at(i + 20).normal = XMFLOAT3(0, 0, -1);
	}

	for (UINT i = 0; i < 6; ++i)
	{
		vertices.at(i * 4).texCoord = XMFLOAT2(0.0f, 1.0f);
		vertices.at(i * 4 + 1).texCoord = XMFLOAT2(0.0f, 0.0f);
		vertices.at(i * 4 + 2).texCoord = XMFLOAT2(1.0f, 0.0f);
		vertices.at(i * 4 + 3).texCoord = XMFLOAT2(1.0f, 1.0f);
	}

	//Get indices
	for (size_t i = 0; i < 6; i++)
	{
		indices.push_back(i * 4);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 3);
		indices.push_back(i * 4);
	}

	std::vector<Texture> textures;
	Texture diskTexture(this->device, "Date\\Textures\\fade_black.png", aiTextureType::aiTextureType_NONE);
	textures.push_back(diskTexture);

	return Mesh(this->device, this->deviceContext, vertices, indices, textures, DirectX::XMMatrixIdentity(), "Debugmesh");
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

	return TextureStorageType::None; // No texture existsZ
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
	collision->obb = collision->originobb;
	auto coworldMatrix = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z)
		* XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z)
		* XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	collision->obb.Transform(collision->obb, coworldMatrix);

}

//=============================================================================
//衝突可視化関数
//=============================================================================
void RenderableGameObject::SetCollisionBoxView(bool view)
{
	this->collision->debugmeshflag = !this->collision->debugmeshflag;
}

//=============================================================================
//衝突取る関数
//=============================================================================
CollsionObject* RenderableGameObject::GetCollisionObject()
{
	return this->collision;
}

//=============================================================================
//copy function
//=============================================================================
void RenderableGameObject::DeepCopy(const RenderableGameObject& go)
{

	this->device = go.device;
	this->deviceContext = go.deviceContext;
	this->cb_vs_vertexshader = go.cb_vs_vertexshader;
	this->path = go.path;

	//copy collision
	collision = new CollsionObject();

	collision->boneindex = go.collision->boneindex;
	collision->collisionoffsetpos = go.collision->collisionoffsetpos;
	collision->collisionoriginextents = go.collision->collisionoriginextents;
	collision->collisionoriginrot = go.collision->collisionoriginrot;
	collision->collisionuse = go.collision->collisionuse;
	collision->ct = go.collision->ct;
	collision->debugmesh = go.collision->debugmesh;
	collision->debugmeshflag = go.collision->debugmeshflag;
	collision->obb = go.collision->obb;
	collision->oritransform = go.collision->oritransform;

	model = go.model;

	this->SetPosition(go.pos);
	this->SetRotation(go.pos);
	this->SetScale(go.scale);

	this->worldMatrix = go.worldMatrix;
}
