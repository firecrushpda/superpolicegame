//=============================================================================
//
// �O���t�B�b�N�X [Graphics.h]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#pragma once
#include <codecvt>
#include "AdapterReader.h"
#include "Shaders.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include "Camera3D.h"
#include "..\\Timer.h"
#include "ImGUI\\imgui.h"
#include "ImGUI\\imgui_impl_win32.h"
#include "ImGUI\\imgui_impl_dx11.h"
#include "RenderableGameObject.h"
#include "Light.h"
#include "Camera2D.h"
#include "Sprite.h"
#include "SkyBox.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Car.h"
#include "CarAIController.h"
#include "Title.h"
#include "Fade.h"
#include "Editor.h"

enum GameState
{
	title,
	game,
	tutorial,
	score,
	editor
};

class Graphics
{
public:
	//������
	bool Initialize(HWND hwnd, int width, int height);

	//�����_
	void RenderFrame();

	//�J����3D
	Camera3D Camera3D;

	//�J����2d
	Camera2D camera2D;

	//2d�p
	Title title;
	Fade fade;
	
	unsigned int tutorialtexno = 0;
	Sprite tutorial;
	Sprite tutorial2;
	Sprite tutorial_background;

	Sprite score;

	//�X�e�[�W
	RenderableGameObject stage;

	//�X�e�[�W
	RenderableGameObject gameroad;

	//�X�e�[�W
	RenderableGameObject girl;


	//�X�e�[�W
	RenderableGameObject test;

	std::vector<RenderableGameObject*> mapgo;

	//
	Car car;
	Car chasecar;
	CarAIController* cac;

	RenderableGameObject quad;

	//�X�e�[�W
	SkyBox skybox;

	//���C�g
	Light light;

	//�T�|�[�gUI�`��
	bool showImgui = false;

	//�[�x�X�e�[�g�L�������ݒu
	void SetDepthEnable(bool Enable);

	//game status
	GameState gs;
	GameState tempgs;

	//prerender
	bool InitializeIBLStatus();
	void Fade(GameState gs);

	//editor
	Editor m_editor;

	//windows screen
	int windowWidth = 0;
	int windowHeight = 0;

	void ResetTitle();
	void ResetGame();

	void EditorRayCast(XMFLOAT2 mousepos);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContent();
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	bool InitalizeBuffers();

	
	bool InitializeEffekseer();
	void EffekseerUpdate();
	void EffekseerDraw();

	void LoadMap();

	Microsoft::WRL::ComPtr<ID3D11Device> device;//�_�C���N�g�G�N�X�f�o�C�X
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//�_�C���N�g�G�N�X�f�o�C�X�R���e�X�g
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//�_�C���N�g�G�N�X�X���b�v �`�F�[��
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//�_�C���N�g�G�N�X�����_�^�[�Q�b�g�����[


	VertexShader vertexshader_2d;//2d�o�[�e�b�N�X�V�F�[�_�[
	VertexShader vertexshader;//�o�[�e�b�N�X�V�F�[�_�[
	VertexShader vertexshader_skyBox;//skybox�o�[�e�b�N�X�V�F�[�_�[
	PixelShader pixelshader_2d;//2d�s�N�Z���V�F�[�_�[
	PixelShader pixelshader;//�s�N�Z���V�F�[�_�[
	PixelShader pixelshader_skyBox;//skybox�s�N�Z���V�F�[�_�[
	PixelShader pixelshader_nolight;//�s�N�Z���V�F�[�_�[
	PixelShader ConvolutionPixelShader;
	PixelShader PrefilterMapPixelShader;
	PixelShader IntegrateBRDFPixelShader;

	//�Ή�����萔�o�b�t�@
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	
	ConstantBuffer<CB_Bone_Info> cb_bone_info;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_IBLSTATUS> cb_ps_iblstatus;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;//�[�x�X�e���V���r���[
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;// �[�x�X�e���V���o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;// �[�x�X�e���V���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthUnenableStencilState;// �[�x�X�e���V���X�e�[�g

	//���X�^���C�U�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

	//�u�����h�X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//�T���v���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brdfLUT;

	//�^�C�}�[
	Timer fpsTimer;

	//���C�g
	std::vector<PointLight*> pointLights;
	std::vector<SpotLight*> spotLights;

	//IBL
	ID3D11Texture2D* skyIBLtex;
	//ID3D11RenderTargetView* skyIBLRTV[6];
	ID3D11ShaderResourceView* skyIBLSRV;

	ID3D11Texture2D* envMaptex;
	//ID3D11RenderTargetView* envMapRTV[6];
	ID3D11ShaderResourceView* envMapSRV;

	ID3D11Texture2D* brdfLUTtex;
	//ID3D11RenderTargetView* brdfLUTRTV;
	ID3D11ShaderResourceView* brdfLUTSRV;

	//effekseer
	//Effekseer::ManagerRef manager;
	//Effekseer::EffectRef effect;
	//EffekseerRendererDX11::RendererRef renderer;
	//int32_t time = 0;
	//Effekseer::Handle handle = 0;
};