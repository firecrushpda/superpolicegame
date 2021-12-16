//=============================================================================
//
// メッシュ [Mesh.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Mesh.h"
#include<DirectXMath.h>
#include<DirectXPackedVector.h>
using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

//=============================================================================
// 初期化
//=============================================================================
Mesh::Mesh(ID3D11Device * device, ID3D11DeviceContext * deviceContext, std::vector<Vertex3D>& vertices, std::vector<DWORD>& indices, std::vector<Texture> & textures, const DirectX::XMMATRIX & transformMatrix,std::string name)
{
	this->deviceContext = deviceContext;
	this->textures = textures;
	this->transformMatrix = transformMatrix;
	this->vertices = vertices;
	this->indices = indices;
	this->name = name;

	HRESULT hr = this->vertexbuffer.Initialize(device, deviceContext, vertices.data(), vertices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertex buffer for mesh.");

	hr = this->indexbuffer.Initialize(device, indices.data(), indices.size());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize index buffer for mesh.");
	
}

//=============================================================================
// 初期化
//=============================================================================
Mesh::Mesh(const Mesh & mesh)
{
	this->deviceContext = mesh.deviceContext;
	this->indexbuffer = mesh.indexbuffer;
	this->vertexbuffer = mesh.vertexbuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
	this->vertices = mesh.vertices;
	this->indices = mesh.indices;
	this->name = mesh.name;
}

//=============================================================================
// 描画
//=============================================================================
void Mesh::Draw()
{
	if(isActive){
		UINT offset = 0;

		for (int i = 0; i < textures.size(); i++)
		{
			if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
			{
				this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_SPECULAR)
			{
				this->deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_HEIGHT)
			{
				this->deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
			else if (textures[i].GetType() == aiTextureType::aiTextureType_DISPLACEMENT)
			{
				this->deviceContext->PSSetShaderResources(3, 1, textures[i].GetTextureResourceViewAddress());
				break;
			}
		}

		this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
		this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
		this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
	}
}

void Mesh::BatchDraw()
{
	UINT offset = 0;
	this->deviceContext->IASetVertexBuffers(0, 1, this->vertexbuffer.GetAddressOf(), this->vertexbuffer.StridePtr(), &offset);
	this->deviceContext->IASetIndexBuffer(this->indexbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->DrawIndexed(this->indexbuffer.IndexCount(), 0, 0);
}

void Mesh::SetTextureInfo() {
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == aiTextureType::aiTextureType_DIFFUSE)
		{
			this->deviceContext->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
		else if (textures[i].GetType() == aiTextureType::aiTextureType_SPECULAR)
		{
			this->deviceContext->PSSetShaderResources(1, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
		else if (textures[i].GetType() == aiTextureType::aiTextureType_HEIGHT)
		{
			this->deviceContext->PSSetShaderResources(2, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
		else if (textures[i].GetType() == aiTextureType::aiTextureType_DISPLACEMENT)
		{
			this->deviceContext->PSSetShaderResources(3, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}
}

//=============================================================================
// マトリクス取る
//=============================================================================
const DirectX::XMMATRIX & Mesh::GetTransformMatrix()
{
	return this->transformMatrix;
}

//=============================================================================
// バーテックスバッファ更新
//=============================================================================
void Mesh::RefreshVerTexBuffer(ID3D11DeviceContext* devicecontext, std::vector<Vertex3D> varray, UINT vertexCount)
{
	this->vertexbuffer.Refresh(devicecontext, varray.data(), vertexCount);
}

ID3D11Buffer* Mesh::GetVertexBuffer()
{
	return vertexbuffer.Get();
}

ID3D11Buffer* Mesh::GetIndexBuffer() {
	return indexbuffer.Get();
}

Mesh Mesh::DefaultGeometry(PrimitiveGeomtry type, ID3D11Device * device,
	ID3D11DeviceContext * devicecontent,
	std::string texturepath,
	std::string meshname)
{
	if (type == PrimitiveGeomtry::box)
	{
		// Data to fill
		std::vector<Vertex3D> vertices;
		std::vector<DWORD> indices;

		XMFLOAT3 corners[8] = {
		 { -1.0f, -1.0f,  1.0f },
		 {  1.0f, -1.0f,  1.0f },
		 {  1.0f,  1.0f,  1.0f },
		 { -1.0f,  1.0f,  1.0f },
		 { -1.0f, -1.0f, -1.0f },
		 {  1.0f, -1.0f, -1.0f },
		 {  1.0f,  1.0f, -1.0f },
		 { -1.0f,  1.0f, -1.0f },
		};

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
		Texture diskTexture(device, texturepath, aiTextureType::aiTextureType_NONE);
		textures.push_back(diskTexture);

		return Mesh(device, devicecontent, vertices, indices, textures, DirectX::XMMatrixIdentity(), meshname);
	}
}