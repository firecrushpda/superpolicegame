#pragma once
//=============================================================================
//
// 車スビートUI [CarSpeedUI.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Sprite.h"

class CarSpeedUI
{
public:
	Sprite speedbar;
	Sprite warningUI;
	Sprite flywarningUI;
	Sprite modeUI;
	Sprite catchcarcountBGUI;
	Sprite catchcarcountUI;

	Sprite countdown[2];

	XMFLOAT2 spdigpos = XMFLOAT2(100,550);
	void Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d,float windowwidth, float windowheight);
	void Update(float fillamount, float speed, int catchcount,  int cd);
	void Draw(XMMATRIX orthoMatrix,bool warningUIflag, bool flywarningUIflag,  bool contdownflag);
private:
	Sprite speedbarbackground;
	Sprite speedbarback;

	Sprite speeddigf[3];
	Sprite dot;
	Sprite speeddigb[2];
};
