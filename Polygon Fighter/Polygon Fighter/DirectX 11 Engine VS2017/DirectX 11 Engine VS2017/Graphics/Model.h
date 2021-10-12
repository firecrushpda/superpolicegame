//=============================================================================
//
// �O���t�B�b�N�X [Graphics.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "Mesh.h"
#include <map>
//#include "../FBXLoader.h"
#include "../Timer.h"
#include <DirectXCollision.h>


using namespace DirectX;

//�x�N�g���L�[
struct VectorKey {
	double time;
	DirectX::XMFLOAT3 value;
};

//�N�I�[�^�j�I���L�[
struct QuaternionKey {
	double time;
	DirectX::XMFLOAT4 value;
};

//�m�[�h���
class BoneNode
{
public:
	BoneNode(std::string name);
	DirectX::XMMATRIX transform;//�ʒu
	DirectX::XMMATRIX geotransform = DirectX::XMMatrixIdentity();//�W�I���g���ʒu
	std::string name;//���O
	bool bIsBoneNode;//���ł���
	bool bIsGeoNode = false;//�W�I���g���m�[�h�ł���
	BoneNode* parentnode = nullptr;//�e

	void AddChild(BoneNode* child);
	void AddMeshIndex(unsigned int index);
	bool CheckIfContainChild(std::string cname);
	std::vector<unsigned int> meshesIndex;
	std::vector<BoneNode*> children;
};

//�A�j���[�V�����`�����l���N���X
class AnimChannel
{
public:
	std::string name;//���O
	void AddPositionKey(VectorKey key);
	void AddRotationKey(QuaternionKey key);
	void AddScalingKey(VectorKey key);
	unsigned int NumPositionKeys();
	unsigned int NumRotationKeys();
	unsigned int NumScalingKeys();
	VectorKey& GetPositionKey(unsigned int index);
	QuaternionKey& GetRotationKey(unsigned int index);
	VectorKey& GetScalingKey(unsigned int index);

private:
	std::vector<VectorKey> scaling_keys;//�X�P�[���L�[�A���[
	std::vector<QuaternionKey> rotation_keys;//���[�e�[�V�����L�[�A���[
	std::vector<VectorKey> position_keys;//�|�W�V�����L�[�A���[
};

//���N���X
class Bone
{
public:
	DirectX::XMMATRIX Nodetransform;//�ʒu
	DirectX::XMMATRIX BoneOffset;//�I�t�Z�b�g
	DirectX::XMMATRIX FinalTransformation;//�Ō�ω���
	DirectX::XMMATRIX GeoTransform;//�W�I���g��

	int parent_index = -1;//�e�C���f�b�N�X

	std::string name = "Empty";//���O
	std::vector<Bone*> children;//�q���A���[

	bool CheckIfHaveBoneChild(Bone* cbone);
};

//�A�j���[�^�[�N���X
class Animation
{
public:
	void SetName(std::string name);
	std::string GetName();
	void SetTickRate(double rate);
	double GetTickRate();
	void AddChannel(AnimChannel* channel);
	bool HasChannel(std::string name);
	AnimChannel& GetChannel(std::string name);
	void SetDuration(double ticks);
	double GetDuration();
	std::vector<aiNodeAnim* >aichannels;
private:
	std::string name;//���O
	double tick_rate;//�`�b�N����
	double duration;//�Ԋu
	std::map<std::string, unsigned int> channels_mapping;//�`�����l���}�b�v
	std::vector<AnimChannel*> channels;//�`�����l���Y
	
};

enum AnimationPlayStyle
{
	PlayOnce,
	PlayLoop,
};


enum KeyEventType
{
	STARTBLADECOLLISION,
	CLOSEBLADECOLLISION,
	STARTANIMATION,
	ENDANIMATION,
};

class AnimationKeyEvent
{
public:
	unsigned int clipindex;
	float keytime;

	KeyEventType keyet;
};

enum CollsionType
{
	Player,
	Enemy,
	Blade
};

class CollsionObject
{
public:
	CollsionObject();
	CollsionObject(const CollsionObject & co);
	bool debugmeshflag;
	int boneindex = -1;
	DirectX::XMMATRIX oritransform;
	std::vector<Mesh> debugmesh;
	BoundingBox aabb;
	CollsionType ct;
};

//���f���N���X
class Model
{
public:
	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void Update(float delta_time, const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	void Draw(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);

	//animation function
	void PlayAnimaition(const int num,AnimationPlayStyle aps);
	unsigned int GetCurrentAnimationClipIndex();
	unsigned int GetAnimationClipSize();
	void SetAnimationSpeed(float speed);
	float GetAnimationSpeed();

	//collision
	CollsionObject* GetBladeCollsionObject();
	CollsionObject* GetBodyCollsionObject();

	//mesh
	std::vector<Mesh> GetMesh();

	bool IfHasBone();
	bool m_showskeleton = false;//�f�o�b�O�p

	void operator = (const Model& model) {
		this->m_showskeleton = model.m_showskeleton;
		this->device = model.device;
		this->deviceContext = model.deviceContext;
		this->cb_vs_vertexshader = model.cb_vs_vertexshader;
		this->cb_bone_info = model.cb_bone_info;
		this->preanimpos = model.preanimpos;
		this->curanimpos = model.curanimpos;
		memcpy(&this->mBoneNodeInfo, &model.mBoneNodeInfo, sizeof(model.mBoneNodeInfo));
		memcpy(&this->meshes, &model.meshes, sizeof(model.meshes));
		this->directory = model.directory;
		this->bHasBone = model.bHasBone;
		this->mNumBones = model.mNumBones;
		memcpy(&this->rootbone, &model.rootbone, sizeof(model.rootbone));
		this->m_GlobalInverseTransform = model.m_GlobalInverseTransform;
		this->rootbonetrans = model.rootbonetrans;
		memcpy(&this->mBoneMaping, &model.mBoneMaping, sizeof(model.mBoneMaping));
		memcpy(&this->mBoneInfo, &model.mBoneInfo, sizeof(model.mBoneInfo));
		this->animplayrate = model.animplayrate;
		this->curanim = model.curanim;
		this->animtimer = model.animtimer;
		memcpy(&this->mAnimations, &model.mAnimations, sizeof(model.mAnimations));
		this->animationPlayStyle = model.animationPlayStyle;
		this->isFbxModel = model.isFbxModel;
		memcpy(&this->debugBlocks, &model.debugBlocks, sizeof(model.debugBlocks));
		memcpy(&this->bodyco, &model.bodyco, sizeof(model.bodyco));
		memcpy(&this->bladeco, &model.bladeco, sizeof(model.bladeco));
		//memcpy(&this->testco, &model.testco, sizeof(model.testco));
		this->precoreslut = model.precoreslut;
	}

private:
	//D3DX refer
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;
	ConstantBuffer<CB_Bone_Info> cb_bone_info;

	//transform
	XMFLOAT3 preanimpos = {0,0,0};
	XMFLOAT3 curanimpos = {0,0,0};
	void UpdateAnimationPosition();

	//node
	std::vector<BoneNode*> mBoneNodeInfo;//�m�[�h�A���[

	BoneNode* FindBoneNodeByName(std::string bonename);
	void ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	//meshes
	std::vector<Mesh> meshes;//���b�V���A���[

	bool LoadModel_Assimp(const std::string & filePath);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);

	//texture
	std::string directory = "";

	TextureStorageType DetermineTextureStorageType(const aiScene * pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene * pScene);// const aiScene* pScene);
	int GetTextureIndex(aiString* pStr);

	//bone
	bool bHasBone = false;//��������H
	unsigned int mNumBones = 0;//�C���f�b�N�X
	Bone* rootbone;//���[�g�{��
	DirectX::XMMATRIX m_GlobalInverseTransform;//�����}�g���N�X
	DirectX::XMMATRIX rootbonetrans = DirectX::XMMatrixIdentity();//���[�g�{���}�g���N�X
	std::map<std::string, unsigned int> mBoneMaping;//�{���}�b�v
	std::vector<Bone*> mBoneInfo;//���A���[
	std::vector<DirectX::XMMATRIX> mworkBone;//���ݎg���Ă鍜�A���[�i���g�p

	int FindBoneParentInAllNode(std::string childname);
	void ProcessBone(aiNode * node, const aiScene * pmScene);
	void ProcessRootBone();
	void ReBindBone(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	//animation
	float animplayrate = 1.0f;//�Đ����x
	unsigned int curanim;//���ݍĐ�����A�j���[�V����
	float animtimer = 0.0f;//�A�j���[�V�������v
	std::vector<Animation*> mAnimations;//�A�j���[�V�����A���[
	AnimationPlayStyle animationPlayStyle = AnimationPlayStyle::PlayLoop;

	bool LoadAnimation_Assimp(const std::string & filePath);
	void UpdateAnimation(float dt);
	AnimChannel* FineAnimChannelByName(std::string bonename);
	void ProcessAnimations(const aiScene* ai_scene);
	void BoneTransform(float TimeInSeconds, std::vector<DirectX::XMMATRIX>& Transforms);
	void ReadBoneHeirarchy(float AnimationTime, Bone* bone, const DirectX::XMMATRIX& ParentTransform);
	void CalcInterpolatedRotation(DirectX::XMFLOAT4& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindRotation(float AnimationTime, AnimChannel* pAnimChannel);
	void CalcInterpolatedScaling(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindScaling(float AnimationTime, AnimChannel* panimchannel);
	void CalcInterpolatedPosition(DirectX::XMFLOAT3& Out, float AnimationTime, AnimChannel* channel);
	unsigned int FindPosition(float AnimationTime, AnimChannel* channel);
	

	//fbx sdk parse bone and model(not use in this project )
	//FBXSDK�����@���g�p
	bool isFbxModel = false;
	//std::unique_ptr<FBX_Model>	m_fbxmodel;

	//debug
	std::vector<Mesh> debugBlocks;//�f�o�b�O��
	//Mesh debugblockmesh;
	
	void DisplayNode(const aiScene * scene);
	unsigned int GetChildNodeCount(aiNode* node, unsigned int nodecount);
	void ProcessDebugNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);
	Mesh ProcessDebugMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);
	void LoadBoneDebugBlock(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);

	//collision
	CollsionObject* bodyco = nullptr;
	CollsionObject* bladeco = nullptr;
	//CollsionObject* testco = nullptr;
	DirectX::ContainmentType precoreslut;
	void ProcessCollsion(aiNode * node, const aiScene * pmScene);
	void UpdateCollisionBox(const XMMATRIX & worldMatrix, const XMMATRIX & viewProjectionMatrix);
	//BoundingBox::CreateFromPoints(this->boundingBox, vertices.size(), &(vertices.at(0)), sizeof(XMFLOAT3));

	//tool
	DirectX::XMMATRIX ToMatrix4f(const aiMatrix4x4 *ai_mat);

};


