#include "Car.h"

bool Car::CarInitialize(const std::string & filePath, ID3D11Device * device,
						ID3D11DeviceContext * deviceContext, 
						ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader) 
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	carrender.Initialize(filePath, device, deviceContext, cb_vs_vertexshader);
	carrender.SetGlobalMatirx(DirectX::XMMatrixIdentity());

	LoadBoneDebugBlock(device, deviceContext, cb_vs_vertexshader);

	return true;
}

void Car::Update(float delta_time, const XMMATRIX & viewProjectionMatrix) 
{
	carrender.Update(delta_time, viewProjectionMatrix);

	auto carmat = carrender.GetWorldMatirx();
	UpdateCollisionBox(carmat, viewProjectionMatrix);
}

void Car::Draw( const XMMATRIX & viewProjectionMatrix)
{
	carrender.Draw(viewProjectionMatrix);

	//collision debug block
	if (!(carco == nullptr))
	{
		if (carco->debugmeshflag)
		{
			for (size_t i = 0; i < carco->debugmesh.size(); i++)
			{
				this->cb_vs_vertexshader->data.wvpMatrix = carco->oritransform * carrender.GetWorldMatirx() * viewProjectionMatrix;//carco->oritransform
				this->cb_vs_vertexshader->data.worldMatrix = carco->oritransform * carrender.GetWorldMatirx();//carco->oritransform *
				this->cb_vs_vertexshader->ApplyChanges();
				carco->debugmesh.at(i).Draw();
			}
		}
	}
}



//=============================================================================
//衝突更新処理関数
//=============================================================================
void Car::UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	for (size_t i = 0; i < carco->debugmesh.size(); i++)
	{
		carco->aabb.Center = XMFLOAT3(0, 0, 0);
		carco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
		auto collisonWorldMatrix = carco->oritransform * worldMatrix * viewProjectionMatrix;
		carco->aabb.Transform(carco->aabb, collisonWorldMatrix);
	}
}



//=============================================================================
//デイバッグメッシュ関数
//=============================================================================
//load debug block (obj file)
void Car::LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
	Assimp::Importer importer;

	const aiScene* Scene = importer.ReadFile("Data\\Objects\\debugBlock.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded |
		aiProcess_JoinIdenticalVertices
	);

	if (!Scene)
		return;

	ProcessDebugNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	ProcessCollsion(Scene->mRootNode, Scene);
}

//=============================================================================
//デイバッグメッシュ関数
//=============================================================================
//create debug block fit the array size
void Car::ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		nodeTransformMatrix = DirectX::XMMatrixIdentity();
		debugBlocks.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));

	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessDebugNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
}

//=============================================================================
//当たり判定ブロック
//=============================================================================
void Car::ProcessCollsion(aiNode * node, const aiScene * pmScene) {
	//body collision
	carco = new CollsionObject();
	carco->boneindex = 1;//pelvis index
	carco->ct = CollsionType::Player;
	carco->debugmeshflag = true;
	auto cnode = node->mChildren[0];
	aiMesh* mesh = pmScene->mMeshes[cnode->mMeshes[0]];
	carco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	carco->aabb = BoundingBox();
	carco->oritransform = DirectX::XMMatrixScaling(3.0f, 3.0f, 3.0f);

}

//=============================================================================
//デイバッグメッシュを読み込む
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh Car::ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
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

	return Mesh(this->device, this->deviceContext, vertices, indices, textures, transformMatrix, mesh->mName.data);
}

//=============================================================================
//マテリアル情報処理
//=============================================================================
std::vector<Texture> Car::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene) //const aiScene * pScene
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
TextureStorageType Car::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
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
int Car::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}
