//=============================================================================
//
// �萔�o�b�t�@��� [ConstantBufferType.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include <DirectXMath.h>

//mvp�A���[���h�}�g���N�X
struct CB_VS_vertexshader
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMMATRIX worldMatrix;
};

//��
struct CB_Bone_Info 
{
	DirectX::XMMATRIX bones[255];
};

//2D�}�g���N�X
struct CB_VS_vertexshader_2d
{
	DirectX::XMMATRIX wvpMatrix;
	DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1,1,1,1);
};


struct CB_DirectionalLight
{
	DirectX::XMFLOAT3 colour;
	float ambientStrength;

	DirectX::XMFLOAT3 direction;
	float padding1;
};

struct CB_SpotLight
{
	DirectX::XMFLOAT3 position;
	float padding1;

	DirectX::XMFLOAT3 colour;
	float padding2;

	float attenuationConstant;
	float attenuationLinear;
	float attenuationQuadratic;
	float innerCutoff;

	float outerCutoff;
	DirectX::XMFLOAT3 direction;
};

struct CB_PointLight
{
	DirectX::XMFLOAT3 position;
	float			  strength;
	DirectX::XMFLOAT3 colour;
	float			  radius;
};

struct CB_Material
{
	float shininess;
	float specularity;
	DirectX::XMFLOAT2 padding1;
};

//���C�g
struct CB_PS_light
{
	/*DirectX::XMFLOAT3 ambientLightColor;
	float ambientLightStrength; 

	DirectX::XMFLOAT3 dynamicLightColor; 
	float dynamicLightStrength; 
	DirectX::XMFLOAT3 dynamicLightPosition; 
	float dynamicLightAttenuation_a;
	float dynamicLightAttenuation_b;
	float dynamicLightAttenuation_c;*/

	//PACK_SEAM
	int numPointLights;
	int numSpotLights;
	DirectX::XMFLOAT2 padding1;

	//PACK_SEAM
	int useNormalMapping;
	int useParallaxOcclusionMapping;
	float parallaxOcclusionMappingHeight;
	int fresnel;

	//PACK_SEAM
	CB_Material objectMaterial;

	//PACK_SEAM
	CB_DirectionalLight directionalLight;
	CB_PointLight pointLights[10];
	CB_SpotLight spotLights[20];

	//PACK_SEAM
	DirectX::XMFLOAT3 cameraPosition;
	float padding2;
};

struct CB_PS_IBLSTATUS
{
	DirectX::XMFLOAT4 color;
	float roughness;
	float metallic;
	float dissolveThreshold;
	float dissolvelineWidth;
	DirectX::XMFLOAT4 dissolveColor;
};

struct CB_PS_Dissolve 
{
	
};