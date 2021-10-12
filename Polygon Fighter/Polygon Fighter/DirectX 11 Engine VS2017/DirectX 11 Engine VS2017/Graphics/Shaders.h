//=============================================================================
//
// �o�[�e�b�N�X�V�F�[�_�[ [VertexShader.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "..\\ErrorLogger.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>


class VertexShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * GetShader();
	ID3D10Blob * GetBuffer();
	ID3D11InputLayout * GetInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;//�V�F�[�_�[�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;//���̓��C�A�E�g
};

class PixelShader
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderpath);
	ID3D11PixelShader * GetShader();
	ID3D10Blob * GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;//�V�F�[�_�[
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer;//�V�F�[�_�[�o�b�t�@
};

