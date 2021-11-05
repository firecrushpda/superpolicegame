#pragma once
#include "Sprite.h"

enum FadeState
{
	Fade_none,
	Fade_In,
	Fade_Out,
};

class Fade
{
public:
	Sprite fadesprite;

	FadeState fadestate;

	float rate = 0.01f;

	void InitFade(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, float width, float height);
	void UpdateFade();
	void DrawFade(XMMATRIX orthoMatrix);
	void UninitFade();
private:

};
