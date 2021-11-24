#pragma once
//#include "Graphics.h"
#include "RenderableGameObject.h"
#include "Sprite.h"
#include "Camera3D.h"
#include "MoneyUI.h"

class Npc
{
public:

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	Camera3D* camera3d;
	
	//ステージ 
	//0 no interaction
	//1 active animation
	//2 load passenger
	//3 reach
	unsigned int npcstate = 0;

	unsigned int npcindex;

	RenderableGameObject girl;

	float npctrigger_distance = 5;

	MoneyUI* moneyui;

	CollsionObject starttrigger;
	CollsionObject endtrigger;
	XMFLOAT3 tgpos = XMFLOAT3(200, 0, 200);
	XMFLOAT3 tgrot = XMFLOAT3(0,0,0);
	XMFLOAT3 tgscale = XMFLOAT3(1,1,1);
	bool possignshowflag;
	Sprite possign;

	Timer npctimer;

	bool uiflag = false;
	Sprite npcUI_thank;

	bool Init(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, 
		ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d,Camera3D* camera3d);
	void Update(float dt, const XMMATRIX & viewProjectionMatrix);
	void Draw3D(const XMMATRIX & viewProjectionMatrix);
	void Draw2D(const XMMATRIX & viewProjectionMatrix2d);
	
	void Release();

private:

};