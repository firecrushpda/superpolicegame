#pragma once
#include "Sprite.h"
#include "Car.h"

class MoneyUI
{
public:
	Sprite moneypricetag;
	Sprite digf[6];

	Car* car;

	XMFLOAT2 digpos;
	bool drawflag = false;

	void Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, float windowwidth, float windowheight,Car* car);
	void Update();
	void Draw(XMMATRIX orthoMatrix);

	void Clear();
	void Hide();
	void Show();
private:
	
};
