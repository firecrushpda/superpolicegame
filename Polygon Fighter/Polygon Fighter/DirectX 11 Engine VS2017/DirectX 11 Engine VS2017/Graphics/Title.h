#pragma once
#include "Sprite.h"

class Title
{
public:
	Sprite Bg;
	Sprite startUI;
	Sprite endUI;
	Sprite tutorial;
	Sprite editor;
	
	float cam_rotfrequance = 0.005;
	float cam_updowmflag = true;
	UINT g_gamemode = 0;
	bool alphaflag;
	float alpha;

	void Init(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, float width, float height);
	void Update();
	void Draw(XMMATRIX orthoMatrix);
private:
	
};