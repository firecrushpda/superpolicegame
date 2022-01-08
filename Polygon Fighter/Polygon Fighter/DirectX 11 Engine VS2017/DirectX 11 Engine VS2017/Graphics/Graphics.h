//=============================================================================
//
// グラフィックス [Graphics.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
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
#include "Npc.h"
#include "MoneyUI.h"
#include "../Sound.h"
#include "PhysicsGameObject.h"

enum GameState
{
	title,
	game,
	tutorial,
	score,
	editor,
	catchcar,
};

class Graphics
{
public:
	//初期化
	bool Initialize(HWND hwnd, int width, int height);

	//レンダ
	void RenderFrame();

	//カメラ3D
	Camera3D Camera3D;

	//カメラ2d
	Camera2D camera2D;

	//2d用
	Title title;
	Fade fade;
	
	unsigned int tutorialtexno = 0;
	Sprite tutorial;
	Sprite tutorial2;
	Sprite tutorial_background;
	Sprite score;

	bool b_siflag = false;
	bool b_thiflag = false;
	Sprite shockimage;
	Sprite taihouimage;

	MoneyUI moneyui;

	//ステージ
	RenderableGameObject stage;
	RenderableGameObject titlestage;

	//ステージ
	RenderableGameObject gameroad;
	RenderableGameObject zimen;

	//ステージ
	std::vector<Npc*> npc;
	unsigned int currentnpcindex;

	//ステージ
	RenderableGameObject test;

	//map game object
	bool b_showmapgo = true;
	bool b_showObstaclego = true;
	std::vector<RenderableGameObject*> mapgo;
	std::vector<RenderableGameObject*> obstaclego;
	std::vector<PxRigidDynamic*> obstaclephysics;
	std::vector<RenderableGameObject*> primitivesgo;

	//not use
	std::vector<Mesh> tempmapgodm;
	Mesh TempProcessDebugMesh(const PxGeometry& geom);

	//
	Car car;
	Car chasecar;
	Car chasecar1;
	Car chasecar2;
	CarAIController* cac;

	RenderableGameObject quad;

	//ステージ
	SkyBox skybox;

	//ライト
	Light light;

	//サポートUI描画
	bool showImgui = false;

	//深度ステート有効無効設置
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

	//game score
	unsigned int scorepageindex = 0;
	float scorepagetimer = 0.0f;
	float gamescore = 0;
	//float gamescore_catch = 0;
	//float gamescore_ride = 0;
	Sprite score01;
	Sprite rank;
	int rank1idx;
	Sprite rank1;
	Sprite catchcardig;
	Sprite scoredigf[6];
	Sprite tscoredigf[6];

	int top1[3];
	Sprite top1name[3];
	Sprite top1scoredigf[6];
	float top1score;
	int top2[3];
	Sprite top2name[3];
	Sprite top2scoredigf[6];
	float top2score;
	int top3[3];
	Sprite top3name[3];
	Sprite top3scoredigf[6];
	float top3score;
	int top4[3];
	Sprite top4name[3];
	Sprite top4scoredigf[6];
	float top4score;
	bool editname = false;
	int editidx = -1;

	void LoadScore();
	void CompareScore();

	//game pause
	bool gamepause = false;
	Timer gamepausetimer;
	bool catchcar_animationsequenceflag = false;
	int catchcar_animaitonindex = 0;
	int catchcar_animaitonmax = 3;

	//flag
	bool b_debugUIflag = true;
	bool b_UIflag = true;

	void ResetTitle();
	void ResetGame();
	void ResetScore();

	void EditorRayCast(XMFLOAT2 mousepos);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContent();
	ConstantBuffer<CB_VS_vertexshader> cb_vs_vertexshader;

	Sound* m_Sound;

	//physics
	bool b_drawphysics = false;
	PhyGameObject pgotest;
	PhysXBasic physxbase;

private:
	bool InitializeDirectX(HWND hwnd);
	bool InitializeShaders();
	bool InitializeScene();
	bool InitalizeBuffers();

	
	bool InitializeEffekseer();
	void EffekseerUpdate();
	void EffekseerDraw();

	void LoadMap();
	void LoadObstacle();
	void LoadNpc();
	int CampareMapname(std::string name);

	Microsoft::WRL::ComPtr<ID3D11Device> device;//ダイレクトエクスデバイス
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;//ダイレクトエクスデバイスコンテスト
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;//ダイレクトエクススワップ チェーン
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;//ダイレクトエクスレンダターゲットヴュー


	VertexShader vertexshader_2d;//2dバーテックスシェーダー
	VertexShader vertexshader;//バーテックスシェーダー
	VertexShader vertexshader_skyBox;//skyboxバーテックスシェーダー
	PixelShader pixelshader_2d;//2dピクセルシェーダー
	PixelShader pixelshader;//ピクセルシェーダー
	PixelShader pixelshader_skyBox;//skyboxピクセルシェーダー
	PixelShader pixelshader_nolight;//ピクセルシェーダー
	PixelShader ConvolutionPixelShader;
	PixelShader PrefilterMapPixelShader;
	PixelShader IntegrateBRDFPixelShader;

	//対応する定数バッファ
	ConstantBuffer<CB_VS_vertexshader_2d> cb_vs_vertexshader_2d;
	
	ConstantBuffer<CB_Bone_Info> cb_bone_info;
	ConstantBuffer<CB_PS_light> cb_ps_light;
	ConstantBuffer<CB_PS_IBLSTATUS> cb_ps_iblstatus;


	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;//深度ステンシルビュー
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;// 深度ステンシルバッファ
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;// 深度ステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthUnenableStencilState;// 深度ステンシルステート

	//ラスタライザステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState_CullFront;

	//ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;

	//サンプルステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pinkTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> grassTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pavementTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brdfLUT;
	
	Sprite disslovenoise;

	//タイマー
	Timer fpsTimer;

	//ライト
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