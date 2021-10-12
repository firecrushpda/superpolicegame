//=============================================================================
//
// �����_�I�u�W�F�N�g���� [RenderableGameObject.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#include "RenderableGameObject.h"

//=============================================================================
// ������
//=============================================================================
bool RenderableGameObject::Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	if (!model.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);//�ʒu
	this->SetRotation(0.0f, 0.0f, 0.0f);//��]
	this->UpdateMatrix();
	return true;
}

//=============================================================================
//�`��
//=============================================================================
void RenderableGameObject::Draw(const XMMATRIX & viewProjectionMatrix)
{
	model.Draw(this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// �}�g���N�X�X�V����
//=============================================================================
void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}

//=============================================================================
// �X�V����
//=============================================================================
void RenderableGameObject::Update(float dt, const XMMATRIX & viewProjectionMatrix)
{
	model.Update(dt, this->worldMatrix, viewProjectionMatrix);
}

//=============================================================================
// ���̃t���b�O
//=============================================================================
void RenderableGameObject::SetSkeletonDebugFlag() 
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// �A�j���[�V�����Đ�
//=============================================================================
void RenderableGameObject::PlayAnimation(unsigned int num,AnimationPlayStyle aps) 
{
	model.PlayAnimaition(num, aps);
}

//=============================================================================
// ���ݍĐ�����A�j���[�V����
//=============================================================================
unsigned int RenderableGameObject::GetCurrentAnimationClipIndex()
{
	return model.GetCurrentAnimationClipIndex();
}

//=============================================================================
// �A�j���[�V������
//=============================================================================
unsigned int RenderableGameObject::GetAnimationClipSize()
{
	return model.GetAnimationClipSize();
}

//=============================================================================
// �A�j���[�V�����Đ��X�s�[�h�ݒu
//=============================================================================
void RenderableGameObject::SetAnimationSpeed(float speed)
{
	model.SetAnimationSpeed(speed);
}

//=============================================================================
// �A�j���[�V�����Đ��X�s�[�h���
//=============================================================================
float RenderableGameObject::GetAnimationSpeed() {
	return model.GetAnimationSpeed();
}

//=============================================================================
// ���̓����蔻��{�b�N�X�����
//=============================================================================
CollsionObject* RenderableGameObject::GetBladeCollsionObject() {
	return model.GetBladeCollsionObject();
}

//=============================================================================
// �̂̓����蔻��{�b�N�X�����
//=============================================================================
CollsionObject* RenderableGameObject::GetBodyCollsionObject() {
	return model.GetBodyCollsionObject();
}

//=============================================================================
// �������邩
//=============================================================================
void RenderableGameObject::ShowBones()
{
	model.m_showskeleton = !model.m_showskeleton;
}

//=============================================================================
// ���b�V�����
//=============================================================================
std::vector<Mesh> RenderableGameObject::GetMesh() {
	return this->model.GetMesh();
}