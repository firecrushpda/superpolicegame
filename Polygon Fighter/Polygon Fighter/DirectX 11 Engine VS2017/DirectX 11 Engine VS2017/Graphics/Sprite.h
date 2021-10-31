//=============================================================================
//
// �X�v���C�g [Sprite.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "GameObject2D.h"
#include "Texture.h"
#include "ConstantBuffer.h"
#include <string>
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Vertex.h"

class Sprite : public GameObject2D
{
public:
	bool Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, float width, float height, std::string spritePath, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d);
	void Draw(XMMATRIX orthoMatrix); //2d camera orthogonal matrix
	float GetWidth();
	float GetHeight();

	void UpdateFillAmount(float rate);
	void UpdateUV(std::vector<Vertex2D> vertexData);
private:
	void UpdateMatrix() override;

	ConstantBuffer<CB_VS_vertexshader_2d> * cb_vs_vertexshader_2d = nullptr;//�萔�o�b�t�@
	XMMATRIX worldMatrix = XMMatrixIdentity();//�ʒu

	std::unique_ptr<Texture> texture;//�e�N�X�`��
	ID3D11DeviceContext * deviceContext = nullptr;

	IndexBuffer indices;//�C���f�b�N�X�o�b�t�@
	VertexBuffer<Vertex2D> vertices;//�o�[�e�b�N�X�A���[
};