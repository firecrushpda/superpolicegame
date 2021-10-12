//=============================================================================
//
// ���f������ [Model.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#include "Model.h"
#include "../ErrorLogger.h"

using namespace DirectX;

#define ANIM_FIGHTIDLE				(4)
#define ANIM_STANDTOFIGHTIDLE		(5)
#define ANIM_BLOCKLEFT				(1)
#define ANIM_BLOCKRIGHT				(2)
#define ANIM_BLOCKIDLE				(3)
#define ANIM_CHARGEATTACK			(0)
#define ANIM_STANDIDLE				(6)
#define ANIM_DEADSTYLE1				(7)

//=============================================================================
// ���f���������֐�
//=============================================================================
bool Model::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	//���f����p�̍����o�b�t�@
	HRESULT hr = this->cb_bone_info.Initialize(this->device, this->deviceContext);
	COM_ERROR_IF_FAILED(hr, "Failed to initialize constant buffer.");

	try
	{
		//FBX SDK����FBX����ǂݍ���
		//���΂炭�o�b�O������܂��̂Ŏg���Ȃ�
		//load mesh
		//if (StringHelper::IsContain(filePath, ".FBX")) {
		//	isFbxModel = true;
		//	m_fbxmodel = std::make_unique<FBX_Model>();
		//	m_fbxmodel->Create(device, deviceContext, cb_vs_vertexshader, filePath.c_str());
		//}
		//else {
			//assimp load model
			//assimp���烂�f������ǂݍ���
			//if (!this->LoadModel_Assimp(filePath))
			//	return false;
		//}

		//assimp load model
		//assimp���烂�f������ǂݍ���
		if (!this->LoadModel_Assimp(filePath))
			return false;
		
		//�������f���͍���t���Ă���Ȃ�e���̈ʒu��\�����鏬����ǂݍ���
		//load debug block for the bone 
		if (bHasBone)
		{
			LoadBoneDebugBlock(device, deviceContext, cb_vs_vertexshader);
		}
		
		//�A�j���[�V������ǂݍ���
		//load animation
		std::vector<std::string> pathes = {
			//action
			"Data\\Objects\\samurai\\Animations\\Action\\ChargeAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\GetHit\\Block_GetHit_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\GetHit\\Block_GetHit_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\BlockIdle.FBX",
			/*"Data\\Objects\\samurai\\Animations\\Action\\Dash_Back.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Front.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dash_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Back.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Front.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Left.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Dodge_Right.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HardAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HitBlockLeft.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\HitBlockRight.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\LightAttk4.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\MediumAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\RoundKick.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\SideKick.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Sprint_LightAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\Sprint_StrongAttk.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabInChest_Attempt.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabInChest_Hit.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StabOnFloor.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk1.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk2.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk3.FBX",
			"Data\\Objects\\samurai\\Animations\\Action\\StrongAttk4.FBX",*/

			//basic
			"Data\\Objects\\samurai\\Animations\\Basic\\FightIdle.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\StandIdle_To_FightIdle.FBX",
			"Data\\Objects\\samurai\\Animations\\Basic\\StandIdle.FBX",

			//dead
			"Data\\Objects\\samurai\\Animations\\GetHit\\Dead1.FBX"
			
		};
		if (StringHelper::IsContain(filePath,"samurai"))
		{
			for (size_t i = 0; i < pathes.size(); i++)
			{
				if (!this->LoadAnimation_Assimp(pathes.at(i)))
				{
					return false;
				}
			}
		}
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	return true;
}

//=============================================================================
// ���f���`��֐�
//=============================================================================
void Model::Draw(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	this->deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader->GetAddressOf());
	this->deviceContext->VSSetConstantBuffers(1, 1, this->cb_bone_info.GetAddressOf());
	//if (isFbxModel)
	//{
	//	//fbx sdk draw
	//	//fbx sdk���f���`��
	//	//m_fbxmodel->Draw(worldMatrix, viewProjectionMatrix);
	//}
	//else
	//{
		//assimp���f���`��
		if (!m_showskeleton)//�f�o�b�O�t���b�O
		{
			for (int i = 0; i < meshes.size(); i++)
			{
				//Update Constant buffer with WVP Matrix
				this->cb_vs_vertexshader->data.wvpMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;//
				this->cb_vs_vertexshader->data.worldMatrix = m_GlobalInverseTransform * meshes[i].GetTransformMatrix() * worldMatrix;//

				this->cb_vs_vertexshader->ApplyChanges();
				meshes[i].Draw();
			}
		}
		
		//�f�o�b�O���f���`��
		if (bHasBone == true && m_showskeleton == true)//
		{
			for (size_t i = 0; i < debugBlocks.size(); i++)
			{
				//Update Constant buffer with WVP Matrix
				this->cb_vs_vertexshader->data.wvpMatrix = debugBlocks[i].GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
				this->cb_vs_vertexshader->data.worldMatrix = debugBlocks[i].GetTransformMatrix() * worldMatrix;

				this->cb_vs_vertexshader->ApplyChanges();
				debugBlocks[i].Draw();
			}
		}

		//collision debug block
		if (!(bodyco == nullptr))
		{
			if (bodyco->debugmeshflag)
			{
				for (size_t i = 0; i < bodyco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = bodyco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = bodyco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					bodyco->debugmesh.at(i).Draw();
				}
			}
		}
		
		if (!(bladeco == nullptr))
		{
			if (bladeco->debugmeshflag)
			{
				for (size_t i = 0; i < bladeco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = bladeco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = bladeco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					bladeco->debugmesh.at(i).Draw();
				}
			}
		}

		/*if (!(testco == nullptr))
		{
			if (testco->debugmeshflag)
			{
				for (size_t i = 0; i < testco->debugmesh.size(); i++)
				{
					this->cb_vs_vertexshader->data.wvpMatrix = testco->debugmesh.at(i).GetTransformMatrix() * worldMatrix * viewProjectionMatrix;
					this->cb_vs_vertexshader->data.worldMatrix = testco->debugmesh.at(i).GetTransformMatrix() * worldMatrix;
					this->cb_vs_vertexshader->ApplyChanges();
					testco->debugmesh.at(i).Draw();
				}
			}
		}*/
		
	//}
}

//=============================================================================
// ���f���ǂݍ��ފ֐�
//=============================================================================
bool Model::LoadModel_Assimp(const std::string & filePath)
{
	//Assimp���C�u���������f�����ǂݍ���
	Assimp::Importer importer;
	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* Scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_TransformUVCoords |
		aiProcess_ConvertToLeftHanded |
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
		);

	if (!Scene)
		return false;

	//����̃��f���ɂ�Z������ɂ��܂�
	//���̂��߂Ɏ�������O�����ŉ�]�����܂�
	//this model export with Z-Up setting 
	//so I change it manual
	m_GlobalInverseTransform = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
	);

	//�m�[�h����
	this->ProcessNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	//����񏈗�
	this->ProcessBone(Scene->mRootNode, Scene);

	//���[�g������
	this->ProcessRootBone();

	//�����ăo�C���h
	this->ReBindBone(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());

	//Assimp�V�[���t���[
	importer.FreeScene();
	return true;
}

//=============================================================================
// �A�j���[�V�����ǂݍ��ފ֐�
//=============================================================================
bool Model::LoadAnimation_Assimp(const std::string & filePath)
{
	//Assimp���C�u�������A�j���[�V�������ǂݍ���
	Assimp::Importer importer;

	//importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

	const aiScene* Scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals|
		aiProcess_GenNormals|
		aiProcess_FixInfacingNormals|
		aiProcess_Triangulate |
		aiProcess_TransformUVCoords|
		aiProcess_SplitLargeMeshes |
		aiProcess_JoinIdenticalVertices
		| aiProcess_ConvertToLeftHanded
		);// 
	
	if (!Scene)
		return false;

	//�A�j���[�V�������������܂��B
	this->ProcessAnimations(Scene);

	//Assimp�V�[���t���[
	importer.FreeScene();
}

//=============================================================================
// �m�[�h�ǂݍ��ފ֐�
//assimp�̃m�[�h�K�w�ɂ�2��ނ�����܂��B
//���̈��
//*************************
//Body
// Body_$AssimpFbx$_GeometricTranslation
//	Body_$AssimpFbx$_Scaling
//   Body_$AssimpFbx$_Rotation
//    Body_$AssimpFbx$_Translation
//Fingers
//*************************
//���̓��
//*************************
//Body
//Fingers
//*************************
//�������̓���[�h�������������Ȃ�AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS��true�ɂ���
//����false�ɂ��܂��B
//Assimp���C�u�������_�~�[�m�[�h�������I�ɕt���Ă��܂�
//���̃m�[�h�ɂ̓m�[�h�̐e�֌W���L�^���Ă��܂��A���ʏ��������Ȃ���΂Ȃ�Ȃ�
//����̂�����"Parentname*_*ChildName" �Ƃ����`���O��ۑ��������L�^���܂��B
//=============================================================================
//process assimp node in heirachy
//there is two different type node name parsed by assimp
//the first one be like 
//*************************
//Body
// Body_$AssimpFbx$_GeometricTranslation
//	Body_$AssimpFbx$_Scaling
//   Body_$AssimpFbx$_Rotation
//    Body_$AssimpFbx$_Translation
//Fingers
//*************************
//the another be like
//*************************
//Body
//Fingers
//*************************
//if you wanna switch these two just make sure the import setting 
//AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS option is false
//I chose the first one cus it can easily get GeometricTranslation of the node
//Assimp create dummy node which name is ""
//this node contain only parent-child relation and a transform
//I threat this node as "Parentname*_*ChildName" and store all the information;
void Model::ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	 BoneNode* tgnode;
	if (node->mName.data && *node->mName.data != '\0')
	{
		//AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS�I�v�V������false�ɂȂ�܂�����
		//Assimp�̃m�[�h���O��"�m�[�h���O_$AssimpFbx$_Transform"�Ƃ����`�ɂ��܂��B
		std::vector<std::string>nameSplit;
		StringHelper::Split(node->mName.data, nameSplit, "_$AssimpFbx$_");

		tgnode = FindBoneNodeByName(nameSplit[0]);
		if (tgnode == nullptr)
		{
			tgnode = new BoneNode(nameSplit[0]); 
			mBoneNodeInfo.emplace_back(tgnode);
			tgnode->transform = ToMatrix4f(&node->mTransformation);
		}
		else
		{
			if (StringHelper::IsContain(node->mName.data, "Geometric"))
			{
				tgnode->bIsGeoNode = true;
				tgnode->geotransform *= ToMatrix4f(&node->mTransformation);
			}
			else
			{
				tgnode->transform *= ToMatrix4f(&node->mTransformation);
			}
		}
	}
	else
	{
		//�_�~�[�m�[�h�ɂ��Ă̓��ʏ���
		//empty node temp solution
		aiString name;
		name.Append(node->mParent->mName.data);
		name.Append("*_*");
		name.Append(node->mChildren[0]->mName.data);
		tgnode = new BoneNode(name.data);
		tgnode->transform = ToMatrix4f(&node->mTransformation);
		mBoneNodeInfo.emplace_back(tgnode);
	}

	//�e�q�֌W��ݒu���܂�
	//set parent child relation
	if (node->mParent != nullptr)
	{
		std::string pname = node->mParent->mName.data;
		if (node->mParent->mName.data && *node->mParent->mName.data != '\0')
		{
			if (StringHelper::IsContain(node->mParent->mName.data, "_$AssimpFbx$_"))
			{
				std::vector<std::string>pnameSplit;
				StringHelper::Split(node->mParent->mName.data, pnameSplit, "_$AssimpFbx$_");
				pname = pnameSplit[0];
			}

			auto pnode = FindBoneNodeByName(pname);
			if (pname != tgnode->name)
			{
				if (!pnode->CheckIfContainChild(tgnode->name))
				{
					pnode->AddChild(tgnode);
					tgnode->parentnode = pnode;
				}
			}
		}
		else
		{
			//�_�~�[�m�[�h�̓��ʏ���
			//empty node add parent child relation
			auto pnode = mBoneNodeInfo.at(mBoneNodeInfo.size() - 2);
			pnode->AddChild(tgnode);
			tgnode->parentnode = pnode;
		}
	}

	//geometry transform�̒l���Stransform�ɂ������܂�
	//we calculate geometry transform with the whole node tree
	DirectX::XMMATRIX nodeTransformMatrix = ToMatrix4f(&node->mTransformation) * parentTransformMatrix;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		DirectX::XMMATRIX temp = nodeTransformMatrix;
		meshes.push_back(this->ProcessMesh(mesh, pmScene, temp));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
	
}

//=============================================================================
//���b�V�������֐�
//�o�[�e�b�N�X���ƃC���f�b�N�X����ۑ����܂��B
//���b�V���Ɋւ�鍜�̏����܂߂Ă��܂��B
//=============================================================================
//process mesh 
//we store vertices indices and texture for the mesh 
//and we create new bone when it is necessary
Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
{
	//�o�[�e�b�N�X�ƃC���f�b�N�X���
	// Data to fill
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	//�o�[�e�b�N�X��񏈗�
	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		//�o�[�e�b�N�X��񂪂����Œ��ڃ}�e�B�A�X����������DirectX�̍��W�n�����킹�܂�
		//we transform vertex position in the begining of impliment so that it can fit DirectX coordination
		auto temp = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.pos), transformMatrix);
		DirectX::XMStoreFloat3(&vertex.pos, temp);

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		//�m�[�}����
		//as well as normal
		//auto temp1 = XMVector3Normalize(DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.normal), transformMatrix));
		//DirectX::XMStoreFloat3(&vertex.normal, temp1);

		if (mesh->mTextureCoords[0])
		{
			//���ڏ��
			//Get tangents
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;

			auto temp2 = XMVector3Normalize(DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vertex.tangent), transformMatrix));
			DirectX::XMStoreFloat3(&vertex.tangent, temp2);

			//we dont use bitangent read by assimp, we calculate in hlsl file
			//Assimp����ǂݍ���bitangent�f�[�^�͎g���Ȃ�
			//vertex.bitangent.x = mesh->mBitangents[i].x;
			//vertex.bitangent.y = mesh->mBitangents[i].y;
			//vertex.bitangent.z = mesh->mBitangents[i].z;

			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
			//auto temp3 = XMVector2Normalize(DirectX::XMVector2Transform(DirectX::XMLoadFloat2(&vertex.texCoord), transformMatrix));
			//DirectX::XMStoreFloat2(&vertex.texCoord, temp3);
			
		}
		
		//������boneindex�ɂ�shader�ɓ��͂��鍜�̏��ł��B
		//��������bone_weights�l��-1����Ȃ��Ȃ�Ȃ���񂪗L���ɂȂ�܂��B
		//�S��bone_weights�̒l���|���Z�ō��킹�ă��b�V���̍Ō�ω��l�𓱂��o��(HLSLshader�t�@�C���ɍs���܂��B)
		vertex.bone_index.w = -1;
		vertex.bone_index.x = -1;
		vertex.bone_index.y = -1;
		vertex.bone_index.z = -1;

		//we judge bone transform by bone weight if bone weight is -1 we dont calculate in shader transform
		vertex.bone_weights.w = -1;
		vertex.bone_weights.x = -1;
		vertex.bone_weights.y = -1;
		vertex.bone_weights.z = -1;

		vertices.push_back(vertex);
		
	}

	//�C���f�b�N�X���
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

	//�����i���O�j
	//�������̃m�[�h�͑S�m�[�h�ƈႢ�܂�
	//���̃m�[�h���e���b�V���ƈႢ�܂��B
	//�e���b�V�����Ђ�����Ԃ��{�����o�^���Ȃ�����o�^���܂��B
	if (mesh->HasBones())
	{
		this->bHasBone = true;

		//iterating all the bones in meshes
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			unsigned int BoneIndex = 0;
			std::string BoneName(mesh->mBones[i]->mName.data);

			if (mBoneMaping.find(BoneName) == mBoneMaping.end()) {
				BoneIndex = mNumBones;
				mNumBones++;
				Bone* bi = new Bone();
				mBoneInfo.push_back(bi);
			}
			else {
				BoneIndex = mBoneMaping[BoneName];
			}
			mBoneMaping[BoneName] = BoneIndex;

			//set bone name
			mBoneInfo[BoneIndex]->name = mesh->mBones[i]->mName.data;
			
			//set bone id and weight in vertex 
			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = mesh->mBones[i]->mWeights[j].mWeight;
				
				//�S��
				if (vertices.at(VertexID).bone_weights.x != -1)
				{
					if (vertices.at(VertexID).bone_weights.y != -1)
					{
						if (vertices.at(VertexID).bone_weights.z != -1)
						{
							if (vertices.at(VertexID).bone_weights.w != -1)
							{
								//full
							}
							else
							{
								vertices.at(VertexID).bone_index.w = (float)BoneIndex;
								vertices.at(VertexID).bone_weights.w = (float)Weight;
							}
						}
						else
						{
							vertices.at(VertexID).bone_index.z = (float)BoneIndex;
							vertices.at(VertexID).bone_weights.z = (float)Weight;
						}
					}
					else
					{
						vertices.at(VertexID).bone_index.y = (float)BoneIndex;
						vertices.at(VertexID).bone_weights.y = (float)Weight;
					}
				}
				else
				{
					vertices.at(VertexID).bone_index.x = (float)BoneIndex;
					vertices.at(VertexID).bone_weights.x = (float)Weight;
				}
			}
		}
	}

	//�e�N�X�`�����
	//get texture information with assimp
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

	//instantiate mesh 
	return Mesh(this->device, this->deviceContext, vertices, indices, textures, DirectX::XMMatrixIdentity(),mesh->mName.data);
}

//=============================================================================
//�f�C�o�b�O���b�V����ǂݍ���
//=============================================================================
//debug block is a simple suqare mesh which represent a bone node position
Mesh Model::ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix)
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
//���[�g�{���ɂ͑S�Ă̍��̐e�ł��B
//Assimp���C�u���������̍���{���Ɉ����Ă��Ȃ��A�܂胁�b�V���̒��Ŋ֘A���Ă��Ȃ�
//�����ŁA�܂����b�V�����J��Ԃ��{�����鎞���̍���������Ȃ�
//�_�~�[�m�[�h�̂��߂����̉�͂�������Ƃ��������B
//****************************
//	 dummy node
//   Hips(parent node)
//		dummy node
//		Spine(child node)
//		dummy node
//		Pelvis(child node)
//****************************
//�{����transform��񂪃_�~�[�m�[�h�ŕۑ����Ă��邩��
//�O��̒T����transform������ɓ���܂��B
//=============================================================================
//the root bone is all bones father 
//Assimp not creat it as a real bone cus it dont move as an animation node in parsed animation file
//so I parse it in manul

//this structure maybe looks weild but it is not my falut
//Assimp create dummy node which name is ""
//this node contain only parent-child relation and a transform
//so the structure of node be like
//****************************
//	 dummy node
//   Hips(parent node)
//		dummy node
//		Spine(child node)
//		dummy node
//		Pelvis(child node)
//****************************
//the real transform information is recorded in dummy node
//so we have to find three times to get the real Hips transform for the rootbone
void Model::ProcessRootBone() {
	rootbone = new Bone();
	if (bHasBone) {
		for (size_t i = 0; i < mBoneInfo.size(); i++)
		{
			if (mBoneInfo.at(i)->parent_index == -1) {
				auto node = FindBoneNodeByName(mBoneInfo.at(i)->name);
				auto pnode = node->parentnode->parentnode->parentnode;

				rootbone->name = "Hips";//node->parentnode->parentnode->name;
				rootbone->Nodetransform = pnode->transform;
				rootbone->children.push_back(mBoneInfo.at(i));
			}
		}
	}
}

//=============================================================================
//�������֐�
//����͑��񍜂̏���ǂݍ���
//���O�A�}�g���N�X���A���֌W��ۑ����܂�
//offset��ReBindBone�֐��ɍĐݒu
//=============================================================================
//process bones store in meshes
//we store name, transform, geotransform and set bone relation
//the bone offset is set by ReBindBone function
void Model::ProcessBone(aiNode * node, const aiScene * pmScene) {
	
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];
		//detail
		//auto geotransform = FindBoneNodeByName(mesh->mName.data)->geotransform;

		if (mesh->HasBones())
		{
			this->bHasBone = true;
			for (unsigned int j = 0; j < mesh->mNumBones; j++) {
				std::string BoneName(mesh->mBones[j]->mName.data);
				
				unsigned int BoneIndex = mBoneMaping[BoneName];
				auto node = FindBoneNodeByName(BoneName);
				
				assert(BoneIndex + 1);

				mBoneInfo[BoneIndex]->Nodetransform = node->transform;
				//mBoneInfo[BoneIndex]->GeoTransform = geotransform;
				//mBoneInfo[BoneIndex]->BoneOffset = ToMatrix4f(&mesh->mBones[j]->mOffsetMatrix);

				mBoneInfo[BoneIndex]->name = mesh->mBones[j]->mName.data;
				mBoneInfo[BoneIndex]->parent_index = FindBoneParentInAllNode(mesh->mBones[j]->mName.data);
				if (mBoneInfo[BoneIndex]->parent_index != -1) {
					auto pbone = mBoneInfo[mBoneInfo[BoneIndex]->parent_index];
					if (!pbone->CheckIfHaveBoneChild(mBoneInfo[BoneIndex]))
					{
						pbone->children.push_back(mBoneInfo[BoneIndex]);
					}
				}
			}
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessBone(node->mChildren[i], pmScene);
	}
}

//=============================================================================
//�����ăo�C���f�B���O�֐�
//Assimp����񋟂���Ă���mOffsetMatrix�������������Ȃ��Ǝv���܂��B
//�����̍ăo�C���f�B���O�Ƃ̓��f����T-Pose����e�m�[�h�̃}�g���N�X����Inverse���{����mOffsetMatrix�Ƃ��ĕۑ����܂�
//=============================================================================
//in my experiment the mOffsetMatrix of Class aiBone is not exactly correct
//it showes the inverse bind matrix which is not transformed by geometry transform
//so it really cause problem when you want to show a geometry transformmed model
//I dont use this parameter in my work 
//I inverse the name matching node transform in geometry transformmed model and store them as new bone offset 
//it worked!
void Model::ReBindBone(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix = ToMatrix4f(&node->mTransformation) * parentTransformMatrix;

	for (size_t j = 0; j < mBoneInfo.size(); j++)
	{
		auto bonename = mBoneInfo.at(j)->name;

		if (bonename == node->mName.data)
		{
			mBoneInfo.at(j)->BoneOffset = DirectX::XMMatrixInverse(nullptr, nodeTransformMatrix);
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ReBindBone(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
}

//=============================================================================
//�A�j���[�^�[����ۑ����܂�
//=============================================================================
//process animations store all the animation info in different AnimChannel
void Model::ProcessAnimations(const aiScene* ai_scene) {

	for (unsigned int i = 0; i < ai_scene->mNumAnimations; i++)
	{
		aiAnimation* ai_animation = ai_scene->mAnimations[i];

		Animation* animation = new Animation();
		animation->SetName(ai_animation->mName.C_Str());
		animation->SetTickRate(ai_animation->mTicksPerSecond);
		animation->SetDuration(ai_animation->mDuration);
		mAnimations.push_back(animation);
		curanim = mAnimations.size() - 1;

		for (unsigned int j = 0; j < ai_animation->mNumChannels; j++)
		{
			aiNodeAnim* ai_channel = ai_animation->mChannels[j];

			//Assimp�����������A�j���[�V�����`�����l������_$AssimpFbx$_��t���閼�O������܂��B
			//�����œ��ʏ������܂�
			if (StringHelper::IsContain(ai_channel->mNodeName.data, "_$AssimpFbx$_"))
			{
				std::vector<std::string>nameSplit;
				StringHelper::Split(ai_channel->mNodeName.data, nameSplit, "_$AssimpFbx$_");

				AnimChannel* channel = FineAnimChannelByName(nameSplit[0]);
				if (channel == nullptr)
				{
					channel = new AnimChannel();
					channel->name = nameSplit[0];
					animation->AddChannel(channel);
				}

				if (nameSplit[1] == "Translation")
				{
					for (unsigned int k = 0; k < ai_channel->mNumPositionKeys; k++)
					{
						aiVectorKey ai_key = ai_channel->mPositionKeys[k];
						VectorKey position_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) }; //{ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
						channel->AddPositionKey(position_key);
					}
				}
				else if (nameSplit[1] == "Rotation")
				{
					for (unsigned int k = 0; k < ai_channel->mNumRotationKeys; k++)
					{
						aiQuatKey ai_key = ai_channel->mRotationKeys[k];
						QuaternionKey rotation_key = { ai_key.mTime,DirectX::XMFLOAT4(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w  } };
						channel->AddRotationKey(rotation_key);
					}
				}
				else if (nameSplit[1] == "Scaling")
				{
					for (unsigned int k = 0; k < ai_channel->mNumScalingKeys; k++)
					{
						aiVectorKey ai_key = ai_channel->mScalingKeys[k];
						VectorKey scaling_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
						channel->AddScalingKey(scaling_key);
					}
				}
			}
			else
			{
				//���ʏ���
				AnimChannel* channel = FineAnimChannelByName(ai_channel->mNodeName.data);
				if (channel == nullptr)
				{
					channel = new AnimChannel();
					channel->name = ai_channel->mNodeName.data;
					animation->AddChannel(channel);
				}

				for (unsigned int k = 0; k < ai_channel->mNumPositionKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mPositionKeys[k];
					VectorKey position_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) }; //DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z)
					channel->AddPositionKey(position_key);
				}

				for (unsigned int k = 0; k < ai_channel->mNumRotationKeys; k++)
				{
					aiQuatKey ai_key = ai_channel->mRotationKeys[k];
					QuaternionKey rotation_key = { ai_key.mTime,DirectX::XMFLOAT4(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z,ai_key.mValue.w  } };
					channel->AddRotationKey(rotation_key);
				}

				for (unsigned int k = 0; k < ai_channel->mNumScalingKeys; k++)
				{
					aiVectorKey ai_key = ai_channel->mScalingKeys[k];
					VectorKey scaling_key = { ai_key.mTime ,DirectX::XMFLOAT3(ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z) };//{ ai_key.mValue.x,ai_key.mValue.y,ai_key.mValue.z } };
					channel->AddScalingKey(scaling_key);
				}
			}
		}
	}
}

//=============================================================================
//�e�N�X�`����񏈗�
//=============================================================================
TextureStorageType Model::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMat, unsigned int index, aiTextureType textureType)
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
//�}�e���A����񏈗�
//=============================================================================
std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene) //const aiScene * pScene
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
				Texture embeddedIndexedTexture(	this->device,
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
//�e�N�X�`���C���f�b�N�X���
//=============================================================================
int Model::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}

//=============================================================================
//�c�[���֐�
//Assimp�}�g���N�X����DirectX�}�g���N�X��
//=============================================================================
//we cant change aiMatrix4x4 matrix smoothly into DirectX::XMMATRIX
DirectX::XMMATRIX Model::ToMatrix4f(const aiMatrix4x4 *ai_mat)
{
	DirectX::XMMATRIX a = DirectX::XMMatrixSet(
		ai_mat->a1, ai_mat->a2, ai_mat->a3, ai_mat->a4,
		ai_mat->b1, ai_mat->b2, ai_mat->b3, ai_mat->b4,
		ai_mat->c1, ai_mat->c2, ai_mat->c3, ai_mat->c4,
		ai_mat->d1, ai_mat->d2, ai_mat->d3, ai_mat->d4);
	DirectX::XMMATRIX b = DirectX::XMMatrixTranspose(a);
	return b;
}

//=============================================================================
//�f�C�o�b�O���b�V���֐�
//=============================================================================
//load debug block (obj file)
void Model::LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader) {
	Assimp::Importer importer;

	const aiScene* Scene = importer.ReadFile("Data\\Objects\\debugBlock.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded|
		aiProcess_JoinIdenticalVertices
		);

	if (!Scene)
		return ;

	ProcessDebugNode(Scene->mRootNode, Scene, DirectX::XMMatrixIdentity());
	
	ProcessCollsion(Scene->mRootNode, Scene);
}

//=============================================================================
//�f�C�o�b�O���b�V���֐�
//=============================================================================
//create debug block fit the bone array size
void Model::ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix)
{
	DirectX::XMMATRIX nodeTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pmScene->mMeshes[node->mMeshes[i]];

		//debugblockmesh = this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity());
		for (size_t j = 0; j < mBoneInfo.size(); j++)
		{
			nodeTransformMatrix = mBoneInfo.at(j)->Nodetransform;
			debugBlocks.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
		}
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessDebugNode(node->mChildren[i], pmScene, nodeTransformMatrix);
	}
}

//=============================================================================
//�����蔻��u���b�N
//=============================================================================
void Model::ProcessCollsion(aiNode * node, const aiScene * pmScene) {
	//body collision
	bodyco = new CollsionObject();
	bodyco->boneindex = 1;//pelvis index
	bodyco->ct = CollsionType::Player;
	bodyco->debugmeshflag = false;
	auto cnode = node->mChildren[0];
	aiMesh* mesh = pmScene->mMeshes[cnode->mMeshes[0]];
	bodyco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	bodyco->aabb = BoundingBox();
	bodyco->oritransform = DirectX::XMMatrixScaling(150.0f, 50.0f, 50.0f);

	//blade collision
	bladeco = new CollsionObject();
	bladeco->boneindex = 51;//right blade index
	bladeco->ct = CollsionType::Blade;
	bladeco->debugmeshflag = false;
	bladeco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	bladeco->aabb = BoundingBox();
	auto bScale = DirectX::XMMatrixScaling(10.0f, 10.0f, 100.0f);
	auto bTrans = DirectX::XMMatrixTranslation(0.0f, 0.0f, 50.0f);
	bladeco->oritransform = bScale * bTrans;

	//test collision
	/*testco = new CollsionObject();
	testco->boneindex = -1;
	testco->ct = CollsionType::Enemy;
	testco->debugmeshflag = true;
	testco->debugmesh.push_back(this->ProcessDebugMesh(mesh, pmScene, DirectX::XMMatrixIdentity()));
	testco->aabb = BoundingBox();
	auto tScale = DirectX::XMMatrixScaling(50.0f, 150.0f, 50.0f);
	auto tTrans = DirectX::XMMatrixTranslation(0.0f, 75.0f, -350.0f);
	testco->oritransform = tScale * tTrans;*/
}

//=============================================================================
//�f�C�o�b�O�m�[�h�c���[�֐�
//=============================================================================
//debug display the structure of Assimp node
void Model::DisplayNode(const aiScene * scene)
{
	//meshes
	unsigned int mnum = scene->mNumMeshes;
	for (unsigned int i = 0; i < mnum; i++)
	{
		auto mesh = scene->mMeshes[i];
		auto mbones = mesh->mNumBones;
		auto hbones = mesh->HasBones();

		//bones
		for (unsigned int k = 0; k < mbones; k++)
		{
			auto bonename = mesh->mBones[k]->mName;
			auto wnum = mesh->mBones[k]->mNumWeights;
			for (unsigned int j = 0; j < wnum; j++)
			{
				auto tweight = mesh->mBones[k]->mWeights[j];
				auto wVertexId = tweight.mVertexId;
				auto mWeight = tweight.mWeight;
			}

		}
	}

	//node
	aiNode* nodenum = scene->mRootNode;
	auto name = nodenum->mName;
	auto transform = nodenum->mTransformation;
	auto mNumMeshes = nodenum->mNumMeshes;
	for (unsigned int i = 0; i < nodenum->mNumMeshes; i++)
	{
		unsigned int meshID = nodenum->mMeshes[i];
	}
	unsigned int nodecount = 1;
	nodecount = GetChildNodeCount(nodenum, nodecount);

	//animation
	auto numanim = scene->mNumAnimations;
	for (unsigned int i = 0; i < numanim; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		auto name = anim->mName;
		auto duration = anim->mDuration;
		auto numchannel = anim->mNumChannels;
		for (unsigned int j = 0; j < numchannel; j++)
		{
			aiNodeAnim* channel = anim->mChannels[j];
			auto nodename = channel->mNodeName;
			aiAnimBehaviour posstate = channel->mPostState;
			aiAnimBehaviour prestate = channel->mPreState;
		}

		auto nummeshchannel = anim->mNumMeshChannels;
		for (unsigned int j = 0; j < nummeshchannel; j++)
		{
			aiMeshAnim* channel = anim->mMeshChannels[j];
			auto name = channel->mName;

		}

		auto nummorpchannel = anim->mNumMorphMeshChannels;
		for (unsigned int j = 0; j < nummorpchannel; j++)
		{
			aiMeshMorphAnim* channel = anim->mMorphMeshChannels[j];
		}
	}
}

//=============================================================================
//�f�C�o�b�O�m�[�h�c���[�֐�
//=============================================================================
unsigned int Model::GetChildNodeCount(aiNode* node, unsigned int nodecount)
{
	unsigned int rtn = nodecount;
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		auto child = node->mChildren[i];
		//infomation
		auto transform = child->mTransformation;
		auto name = child->mName;
		auto mNumMeshes = child->mNumMeshes;
		for (unsigned int i = 0; i < child->mNumMeshes; i++)
		{
			unsigned int meshID = child->mMeshes[i];
		}

		if (child->mNumChildren != 0)
		{
			rtn = GetChildNodeCount(child, rtn);
		}
		
		if (i == node->mNumChildren - 1)
		{
			rtn += node->mNumChildren;
		}
	}
	return rtn;
}

//=============================================================================
//�f�C�o�b�O�m�[�h�c���[�֐�
//=============================================================================
//update bone transform
void Model::BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms)
{
	//���ݎg�Ă���A�j���[�V����
	Animation* mAnimation = this->mAnimations.at(curanim);
	
	//�A�j���[�V������tickrate(�t���[���P��)
	float TicksPerSecond = mAnimation->GetTickRate() != 0 ?
		mAnimation->GetTickRate() : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond / 10.0f;
	animtimer += TimeInTicks * animplayrate;

	if (animationPlayStyle == AnimationPlayStyle::PlayOnce)
	{
		if (animtimer >= mAnimation->GetDuration() - 1)
		{
			animtimer = mAnimation->GetDuration() - 1;
			//set animation end function

			if (curanim == ANIM_STANDTOFIGHTIDLE)
			{
				PlayAnimaition(ANIM_FIGHTIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_BLOCKLEFT)
			{
				PlayAnimaition(ANIM_BLOCKIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_BLOCKRIGHT)
			{
				PlayAnimaition(ANIM_BLOCKIDLE, AnimationPlayStyle::PlayLoop);
			}
			if (curanim == ANIM_CHARGEATTACK)
			{
				PlayAnimaition(ANIM_FIGHTIDLE, AnimationPlayStyle::PlayLoop);
			}
		}
	}
	else if(animationPlayStyle == AnimationPlayStyle::PlayLoop)
	{

	}
	float AnimationTime = fmod(animtimer, mAnimation->GetDuration());
	
	//�o�R�������ԂŃA�j���[�V�����̍������X�V���܂�
	ReadBoneHeirarchy(AnimationTime, rootbone, DirectX::XMMatrixIdentity());//m_GlobalInverseTransform

	//�f�C�o�b�O�|�C���g���X�V���܂�
	Transforms.resize(mBoneInfo.size());
	for (unsigned int i = 0; i < mBoneInfo.size(); i++) {
		Transforms[i] = mBoneInfo[i]->FinalTransformation;
		debugBlocks.at(i).transformMatrix = DirectX::XMMatrixInverse(nullptr, mBoneInfo.at(i)->BoneOffset) * Transforms[i] * m_GlobalInverseTransform;
	}

	//������shader�t�@�C���ɓ���
	//this->deviceContext->VSSetConstantBuffers(1, 1, this->cb_bone_info.GetAddressOf());
	for (unsigned int i = 0; i < Transforms.size(); i++)
	{
		this->cb_bone_info.data.bones[i] = Transforms.at(i);

	}
	this->cb_bone_info.ApplyChanges();

}

//=============================================================================
//���c���[�X�V�֐�
//�o�R�������ԂŃA�j���[�V�����̍������X�V���܂�
//=============================================================================
//iterating bone heirarchy
//set bone transform by animation interpolating time
void Model::ReadBoneHeirarchy(float AnimationTime, Bone* bone, const DirectX::XMMATRIX& ParentTransform)
{
	//�S�m�[�h����^�[�Q�b�g�m�[�h��T���܂�
	auto node = FindBoneNodeByName(bone->name);
	DirectX::XMMATRIX NodeTransformation = node->parentnode->transform;
	DirectX::XMMATRIX GlobalTransformation;

	//�����A�j���[�V�����`�����l����ۑ�������
	AnimChannel* channel = FineAnimChannelByName(bone->name);
	if (channel) {
		//�����_��Scaling�����܂�
		// Interpolate scaling and generate scaling transformation matrix
		DirectX::XMFLOAT3 Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, channel);
		DirectX::XMVECTOR ScalingV = DirectX::XMLoadFloat3(&Scaling);
		DirectX::XMMATRIX ScalingM = DirectX::XMMatrixScalingFromVector(ScalingV);

		//�����_��Rotation�����܂�
		// Interpolate rotation and generate rotation transformation matrix
		DirectX::XMFLOAT4 RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, channel);
		DirectX::XMVECTOR RotationV = DirectX::XMLoadFloat4(&RotationQ);
		RotationV = DirectX::XMVector4Normalize(RotationV);
		DirectX::XMMATRIX RotationM = DirectX::XMMatrixRotationQuaternion(RotationV);

		//�����_��Translation�����܂�
		// Interpolate translation and generate translation transformation matrix
		DirectX::XMFLOAT3 Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, channel);
		DirectX::XMVECTOR TranslationV = DirectX::XMLoadFloat3(&Translation);
		DirectX::XMMATRIX TranslationM = DirectX::XMMatrixTranslationFromVector(TranslationV);

		//����
		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;//srt   
		
	}

	//�e�}�g���N�X���|���Z���܂�
	GlobalTransformation = NodeTransformation * ParentTransform;

	//�Ō�̕ύX�}�g���N�X���v�Z���܂�
	if (mBoneMaping.find(bone->name) != mBoneMaping.end()) {
		unsigned int BoneIndex = mBoneMaping[bone->name];

		mBoneInfo[BoneIndex]->FinalTransformation = mBoneInfo[BoneIndex]->BoneOffset * (GlobalTransformation);
		
	}

	//�q�����J��Ԃ�
	for (size_t i = 0; i < bone->children.size(); i++)
	{
		auto cbone = bone->children.at(i);
		ReadBoneHeirarchy(AnimationTime, cbone, GlobalTransformation);
	}
}

//=============================================================================
//�e����T���֐�
//=============================================================================
//find root bone in bone array
//this bone is not the real root bone 
//we have to find the real on in all node
int Model::FindBoneParentInAllNode(std::string childname)
{
	auto cnode = FindBoneNodeByName(childname);
	auto pnode = cnode->parentnode;
	if (pnode!= nullptr && StringHelper::IsContain(pnode->name, "*_*"))
	{
		pnode = pnode->parentnode;
	}
	if (mBoneMaping.find(pnode->name) == mBoneMaping.end()) 
	{
		return -1;
	}
	else {
		return mBoneMaping[pnode->name];
	}

}

//=============================================================================
//Rotation�T���֐�
//���`��Ԍ���
//=============================================================================
//Interpolate Rotation by AnimationTime
void Model::CalcInterpolatedRotation(DirectX::XMFLOAT4& Out, float AnimationTime, AnimChannel* channel)
{
	// we need at least two values to interpolate...
	auto num = channel->NumRotationKeys();
	if (num == 1) {
		Out = channel->GetRotationKey(0).value;
		return;
	}

	if (num == 0)
	{
		Out = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f,1.0f);
		return;
	}
	unsigned int RotationIndex = FindRotation(AnimationTime, channel);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	auto mrrk = channel->GetRotationKey(RotationIndex);
	auto mnrk = channel->GetRotationKey(NextRotationIndex);
	assert(NextRotationIndex < channel->NumRotationKeys());
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - mrrk.time) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	//XMQuaternionSlerp
	
	DirectX::XMFLOAT4 StartRotationQ = mrrk.value;
	DirectX::XMFLOAT4 EndRotationQ = mnrk.value;
	auto StartRotationV = DirectX::XMLoadFloat4(&StartRotationQ);
	auto EndRotationV = DirectX::XMLoadFloat4(&EndRotationQ);
	DirectX::XMVECTOR result = DirectX::XMQuaternionSlerp(StartRotationV, EndRotationV, Factor);
	//result = DirectX::XMVector4Normalize(result);
	DirectX::XMStoreFloat4(&Out,result);
}

//=============================================================================
//Rotation�L�[�T���֐�
//=============================================================================
//find Rotation key by AnimationTime
unsigned int Model::FindRotation(float AnimationTime, AnimChannel* pAnimChannel)
{
	assert(pAnimChannel->NumRotationKeys() > 0);
	for (unsigned int i = 0; i < pAnimChannel->NumRotationKeys() - 1; i++) {
		if (AnimationTime < (float)pAnimChannel->GetRotationKey(1 + i).time) {
			return i;
		}
	}
	assert(0);
}

//=============================================================================
//Scaling�T���֐�
//���`��Ԍ���
//=============================================================================
//Interpolate Scaling by AnimationTime
void Model::CalcInterpolatedScaling(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel)
{
	auto num = channel->NumScalingKeys();

	if (num == 1) {
		Out = channel->GetScalingKey(0).value;
		return;
	}
	if (num == 0)
	{
		Out = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, channel);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < num);
	auto mrrk = channel->GetScalingKey(ScalingIndex);
	auto mnrk = channel->GetScalingKey(NextScalingIndex);
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - mrrk.time)/ DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	DirectX::XMFLOAT3 Start = mrrk.value;
	DirectX::XMFLOAT3 End = mnrk.value;
	DirectX::XMFLOAT3 Delta = DirectX::XMFLOAT3((End.x - Start.x), (End.y - Start.y), (End.z - Start.z));
	DirectX::XMFLOAT3 result = DirectX::XMFLOAT3((Start.x + Factor * Delta.x), (Start.y + Factor * Delta.y), (Start.z + Factor * Delta.z));
	Out = DirectX::XMFLOAT3(result.x,result.y, result.z);
}

//=============================================================================
//Scaling�L�[�T���֐�
//=============================================================================
//find Scaling key by AnimationTime
unsigned int Model::FindScaling(float AnimationTime, AnimChannel* panimchannel)
{
	assert(panimchannel->NumScalingKeys() > 0);

	for (unsigned int i = 0; i < panimchannel->NumScalingKeys() - 1; i++) {
		if (AnimationTime < (float)panimchannel->GetScalingKey(i + 1).time) {
			return i;
		}
	}
	assert(0);
	return 0;
}

//=============================================================================
//Position�T���֐�
//���`��Ԍ���
//=============================================================================
//Interpolate Position by AnimationTime
void Model::CalcInterpolatedPosition(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel)
{
	auto num = channel->NumPositionKeys();
	if (channel->NumPositionKeys() == 1) {
		Out = channel->GetPositionKey(0).value;
		return;
	}
	if (num == 0)
	{
		Out = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		return;
	}
	unsigned int PositionIndex = FindPosition(AnimationTime, channel);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < num);
	auto mrrk = channel->GetPositionKey(PositionIndex);
	auto mnrk = channel->GetPositionKey(NextPositionIndex);
	float DeltaTime = mnrk.time - mrrk.time;
	float Factor = (AnimationTime - (float)mrrk.time) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);

	DirectX::XMVECTOR p0 = DirectX::XMLoadFloat3(&mrrk.value);
	DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&mnrk.value);
	DirectX::XMVECTOR temp = DirectX::XMVectorLerp(p0, p1, Factor);
	DirectX::XMStoreFloat3(&Out, temp);
}

//=============================================================================
//Position�L�[�T���֐�
//=============================================================================
//find Position key by AnimationTime
unsigned int Model::FindPosition(float AnimationTime, AnimChannel* channel)
{
	for (unsigned int i = 0; i < channel->NumPositionKeys() - 1; i++) {
		if (AnimationTime < (float)channel->GetPositionKey(i + 1).time) {
			return i;
		}
	}

	assert(0);

	return 0;
}

//=============================================================================
//���f���X�V�����֐�
//=============================================================================
//update
void Model::Update(float dt, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	if (this->IfHasBone())
	{
		this->UpdateAnimation(dt);
		this->UpdateCollisionBox(worldMatrix, viewProjectionMatrix);
		this->UpdateAnimationPosition();
	}
}

//=============================================================================
//�A�j���[�V�����X�V�����֐�
//=============================================================================
//update bone animation
void Model::UpdateAnimation(float dt)
{
	float RunningTime = dt / 100.0f;

	this->BoneTransform(RunningTime, mworkBone);


}

//=============================================================================
//�ՓˍX�V�����֐�
//=============================================================================
void Model::UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix)
{
	if (bodyco->boneindex != -1)//pelvis transformation
	{
		auto bodymatrix = debugBlocks.at(bodyco->boneindex).transformMatrix;
		//auto trans = bodymatrix * m_GlobalInverseTransform;

		for (size_t i = 0; i < bodyco->debugmesh.size(); i++) 
		{
			bodyco->debugmesh.at(i).transformMatrix = bodyco->oritransform * bodymatrix;
			bodyco->aabb.Center = XMFLOAT3(0, 0, 0);
			bodyco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
			auto collisonWorldMatrix = bodyco->debugmesh.at(i).transformMatrix * worldMatrix * viewProjectionMatrix;
			bodyco->aabb.Transform(bodyco->aabb, collisonWorldMatrix);
		}
	}

	if (bladeco->boneindex != -1)//pelvis transformation
	{
		auto bladematrix = debugBlocks.at(bladeco->boneindex).transformMatrix;
		//auto trans = bladematrix * m_GlobalInverseTransform;
		for (size_t i = 0; i < bladeco->debugmesh.size(); i++)
		{
			bladeco->debugmesh.at(i).transformMatrix = bladeco->oritransform * bladematrix; //*trans;
			bladeco->aabb.Center = XMFLOAT3(0, 0, 0);
			bladeco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
			auto collisonWorldMatrix = bladeco->debugmesh.at(i).transformMatrix * worldMatrix * viewProjectionMatrix;
			bladeco->aabb.Transform(bladeco->aabb, collisonWorldMatrix);
		}
	}

	//test collision
	/*for (size_t i = 0; i < testco->debugmesh.size(); i++)
	{
		testco->debugmesh.at(i).transformMatrix = testco->oritransform;
		testco->aabb.Center = XMFLOAT3(0, 0, 0);
		testco->aabb.Extents = XMFLOAT3(1.0f, 1.0f, 1.0f);
		testco->aabb.Transform(testco->aabb, testco->debugmesh.at(i).transformMatrix);
	}*/

	//blade - test
	//DirectX::ContainmentType coresult = bladeco->aabb.Contains(testco->aabb);
	//if (precoreslut == 0 && coresult != 0)
	//{
	//	//hit
	//	bladeco->debugmeshflag = true;
	//	testco->debugmeshflag = false;
	//}
	//if (precoreslut != 0 && coresult == 0)
	//{
	//	bladeco->debugmeshflag = false;
	//	testco->debugmeshflag = true;
	//}
	//precoreslut = coresult;
}

//=============================================================================
//�A�j���[�V��������ʒu���ړ�����֐�
//�A�j���[�V�������Đ����鎞���f�����ړ������邩��
//�������炩�̗��R�������ēr���ŃA�j���[�V�����𒆎~���܂�����
//���̌��ʂ����̈ʒu�ɉ������琳�����ʒu�𓾂�
//=============================================================================
void Model::UpdateAnimationPosition() {
	auto pelvisbonetrans = debugBlocks.at(bodyco->boneindex).transformMatrix;
	DirectX::XMFLOAT4X4 mat44;
	XMStoreFloat4x4(&mat44, pelvisbonetrans);
	curanimpos.x = mat44._41;
	curanimpos.y = mat44._42;
}

//=============================================================================
//���f����������̊m�F�֐�
//=============================================================================
bool Model::IfHasBone() 
{
	return this->bHasBone;
}

//=============================================================================
//�^�[�Q�b�g�m�[�h�T���֐�
//=============================================================================
//find node in all node
BoneNode* Model::FindBoneNodeByName(std::string bonename)
{
	for (unsigned int i = 0; i < this->mBoneNodeInfo.size(); i++)
	{
		if (mBoneNodeInfo.at(i)->name == bonename)
		{
			return mBoneNodeInfo.at(i);
		}
	}
	return nullptr;
}


//=============================================================================
//�A�j���[�V�����`�����l���T���֐�
//=============================================================================
AnimChannel* Model::FineAnimChannelByName(std::string bonename)
{
	Animation* animation = mAnimations.at(curanim);
	if (animation->HasChannel(bonename))
	{
		return &animation->GetChannel(bonename);
	}
	return nullptr;
}

//=============================================================================
//�A�j���[�V�����v���C�֐�
//=============================================================================
void Model::PlayAnimaition(const int num, AnimationPlayStyle aps)
{
	animationPlayStyle = aps;
	curanim = num;
	animtimer = 0.0f;

	preanimpos = curanimpos;
	curanimpos = { 0,0,0 };
}

//=============================================================================
//���ݍĐ�����A�j���[�V����
//=============================================================================
unsigned int Model::GetCurrentAnimationClipIndex()
{
	return curanim;
}

//=============================================================================
//�S�ẴA�j���[�V������
//=============================================================================
unsigned int Model::GetAnimationClipSize() {
	return mAnimations.size();
}

//=============================================================================
//�A�j���[�V�����Đ����x�ݒu
//=============================================================================
void Model::SetAnimationSpeed(float speed) {
	animplayrate = speed;
}

//=============================================================================
//�A�j���[�V�����Đ����x���
//=============================================================================
float Model::GetAnimationSpeed() {
	return animplayrate;
}

//-------------------------------
//Animation Function(�A�j���[�V�����N���X�֐�)
//-------------------------------
//���O�ݒu
void Animation::SetName(std::string name) 
{
	this->name = name;
}

//���O�Q�b�g
std::string Animation::GetName()
{
	return this->name;
}

//TickRate�ݒu
void Animation::SetTickRate(double rate)
{
	this->tick_rate = rate;
}

//TickRate�Q�b�g
double Animation::GetTickRate()
{
	return this->tick_rate;
}

//�`�����l������
void Animation::AddChannel(AnimChannel * channel)
{
	channels_mapping[channel->name] = channels.size();
	channels.emplace_back(std::move(channel));
}

//�`�����l���`�F�b�N
bool Animation::HasChannel(std::string name)
{
	auto found = channels_mapping.find(name);
	return (found != channels_mapping.end());
}

//�`�����l���Ƃ�
AnimChannel& Animation::GetChannel(std::string name)
{
	auto found = channels_mapping.find(name);

	assert(found != channels_mapping.end());

	return *channels[found->second];
}

//���ԑѐݒu
void Animation::SetDuration(double ticks)
{
	this->duration = ticks;
}

//���ԑтƂ�
double Animation::GetDuration()
{
	return this->duration;
}


//-------------------------------
//AnimChannel Function(�A�j���[�V�����`�����l���֐�)
//-------------------------------
//Position�L�[�Y��
void AnimChannel::AddPositionKey(VectorKey key)
{
	position_keys.push_back(key);
}

//Rotation�L�[�Y��
void AnimChannel::AddRotationKey(QuaternionKey key)
{
	rotation_keys.push_back(key);
}

//Scaling�L�[�Y��
void AnimChannel::AddScalingKey(VectorKey key)
{
	scaling_keys.push_back(key);
}

//Position�L�[�������
unsigned int AnimChannel::NumPositionKeys()
{
	return position_keys.size();
}

//Rotation�L�[�������
unsigned int AnimChannel::NumRotationKeys()
{
	return rotation_keys.size();
}

//Scaling�L�[�������
unsigned int AnimChannel::NumScalingKeys()
{
	return scaling_keys.size();
}

//Position�L�[�����
VectorKey& AnimChannel::GetPositionKey(unsigned int index)
{
	return position_keys[index];
}

//Rotation�L�[�����
QuaternionKey& AnimChannel::GetRotationKey(unsigned int index)
{
	return rotation_keys[index];
}

//Scaling�L�[�����
VectorKey& AnimChannel::GetScalingKey(unsigned int index)
{
	return scaling_keys[index];
}

//-------------------------------
//BoneNode Function(�m�[�h�N���X�֐�)
//-------------------------------
//���O�ݒu
BoneNode::BoneNode(std::string name)
{
	this->name = name;
}

//�q�m�[�h�Y��
void BoneNode::AddChild(BoneNode* child)
{
	this->children.emplace_back(child);
}

//���b�V���C���f�b�N�X�ݒu
void BoneNode::AddMeshIndex(unsigned int index)
{
	this->meshesIndex.emplace_back(index);
}

//�q�m�[�h�����̒��ł��邩�ǂ���
bool BoneNode::CheckIfContainChild(std::string cname) 
{
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		if (this->children.at(i)->name == cname)
			return true;
	}
	return false;
}

//-------------------------------
//Bone Function(���N���X�֐�)
//-------------------------------
//�q�m�[�h�����̒��ł��邩�ǂ���
bool Bone::CheckIfHaveBoneChild(Bone* cbone) 
{
	for (unsigned int i = 0; i < this->children.size(); i++)
	{
		if (this->children.at(i)->name == cbone->name)
			return true;
	}
	return false;
}

//-------------------------------
//CollsionObject Function(�Փ˔���֐�)
//-------------------------------
CollsionObject::CollsionObject(const CollsionObject & co) {
	this->aabb = co.aabb;
	this->ct = co.ct;
	this->debugmesh = co.debugmesh;
	this->debugmeshflag = co.debugmeshflag;
}

CollsionObject::CollsionObject() {

}

CollsionObject* Model::GetBladeCollsionObject() 
{
	return bladeco;
}

CollsionObject* Model::GetBodyCollsionObject() 
{
	return bodyco;
}

//-------------------------------------------
//mesh(���b�V�����֐�)
//-------------------------------------------
std::vector<Mesh> Model::GetMesh()
{
	return this->meshes;
}