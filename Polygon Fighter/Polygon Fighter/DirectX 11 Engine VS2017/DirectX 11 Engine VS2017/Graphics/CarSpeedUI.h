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

	XMFLOAT2 spdigpos = XMFLOAT2(100,550);
	void Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d);
	void Update(float fillamount, float speed);
	void Draw(XMMATRIX orthoMatrix);
private:
	Sprite speedbarback;

	Sprite speeddigf[3];
	Sprite dot;
	Sprite speeddigb[2];
};
