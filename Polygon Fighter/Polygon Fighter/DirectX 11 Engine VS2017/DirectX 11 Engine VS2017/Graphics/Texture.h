//=============================================================================
//
// �e�N�X�`�� [Texture.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Color.h"
#include <assimp/material.h>

//�e�N�X�`���ۑ����
enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedIndexNonCompressed,
	EmbeddedCompressed,
	EmbeddedNonCompressed,
	Disk
};

class Texture
{
public:
	Texture(ID3D11Device * device, const Color & color, aiTextureType type);
	Texture(ID3D11Device * device, const Color * colorData, UINT width, UINT height, aiTextureType type); //Generate texture of specific color data
	Texture(ID3D11Device * device, const std::string & filePath, aiTextureType type);
	Texture(ID3D11Device * device, const uint8_t* pData, size_t size, aiTextureType type);
	aiTextureType GetType();
	ID3D11ShaderResourceView * GetTextureResourceView();
	ID3D11ShaderResourceView ** GetTextureResourceViewAddress();

private:
	void Initialize1x1ColorTexture(ID3D11Device * device, const Color & colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device * device, const Color * colorData, UINT width, UINT height, aiTextureType type);
	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;//�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;//�����_���\�[�X�r���[
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;//Assimp�e�N�X�`�����
};