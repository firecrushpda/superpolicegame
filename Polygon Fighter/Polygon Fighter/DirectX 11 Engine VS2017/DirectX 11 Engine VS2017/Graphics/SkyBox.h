//=============================================================================
//
// �����_�Q�[���I�u�W�F�N�g [RenderableGameObject.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"

class SkyBox : public GameObject3D
{
public:
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;

	std::string directory = "";

	bool Initialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);

	void Draw(const DirectX::XMMATRIX & viewProjectionMatrix);

	void Update(float dt, const XMMATRIX & viewProjectionMatrix);

	void ProcessNode(aiNode * node, const aiScene * pmScene, const XMMATRIX & parentTransformMatrix);

	VertexBuffer<Vertex3D_SkyBox> vertexbuffer;//�o�[�e�b�N�X�o�b�t�@
	IndexBuffer indexbuffer;//�C���f�b�N�X�o�b�t�@

	std::vector<Vertex3D_SkyBox> vertices;
	std::vector<DWORD> indices;

	DirectX::XMMATRIX m_GlobalInverseTransform = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
	);

	//meshes
	std::vector<Mesh> meshes;//���b�V���A���[

	void ProcessMesh(aiMesh * mesh, const aiScene * pmScene, const XMMATRIX & transformMatrix);

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;//�e�N�X�`��
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView = nullptr;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView1 = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView2 = nullptr;

	DirectX::XMMATRIX ToMatrix4f(const aiMatrix4x4 *ai_mat);

	HRESULT CreateWICTexture2DCubeFromFile(
		ID3D11Device * d3dDevice,
		ID3D11DeviceContext * d3dDeviceContext,
		const std::wstring & cubeMapFileName,
		ID3D11Texture2D ** textureArray,
		ID3D11ShaderResourceView ** textureCubeView,
		bool generateMips);

	HRESULT CreateWICTexture2DCubeFromFile(
		ID3D11Device * d3dDevice,
		ID3D11DeviceContext * d3dDeviceContext,
		const std::vector<std::wstring>& cubeMapFileNames,
		ID3D11Texture2D ** textureArray,
		ID3D11ShaderResourceView ** textureCubeView,
		bool generateMips);

	void ChangeSkyBoxSRV();
protected:

	void UpdateMatrix() override;

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();//�ʒu
};