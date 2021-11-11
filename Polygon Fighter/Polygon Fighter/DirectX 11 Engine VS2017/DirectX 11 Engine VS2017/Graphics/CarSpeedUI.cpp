//=============================================================================
//
// 車スビートUI [CarSpeedUI.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "CarSpeedUI.h"

void CarSpeedUI::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d,float windowwidth,float windowheight) {

	spdigpos = XMFLOAT2(100, windowheight - 50);
	speedbar.Initialize(device, deviceContext, 207, 100, "Data/Textures/speedbar2.png", cb_vs_vertexshader_2d);
	speedbar.SetPosition(XMFLOAT3(0, windowheight - 100, 0));
	speedbarback.Initialize(device, deviceContext, 207, 100, "Data/Textures/speedbar1.png", cb_vs_vertexshader_2d);
	speedbarback.SetPosition(XMFLOAT3(0, windowheight - 100, 0));

	for (size_t i = 0; i < 3; i++)
	{
		speeddigf[2 - i].Initialize(device, deviceContext, 50, 74, "Data/Textures/cartoonnum.png", cb_vs_vertexshader_2d);
		speeddigf[2 - i].SetPosition(spdigpos.x + 25 * i, spdigpos.y, 0);
		speeddigf[2 - i].SetScale(50 * 0.5f, 74 * 0.5f);
	}

	dot.Initialize(device, deviceContext, 50, 50, "Data/Textures/dot.png", cb_vs_vertexshader_2d);
	dot.SetPosition(XMFLOAT3(spdigpos.x + 55.0, spdigpos.y + 10, 0));

	for (size_t i = 0; i < 2; i++)
	{
		speeddigb[i].Initialize(device, deviceContext, 12.5, 74 / 4, "Data/Textures/cartoonnum.png", cb_vs_vertexshader_2d);
		speeddigb[i].SetPosition(spdigpos.x + 85 +12.5 * i, spdigpos.y + 20, 0);
	}

	warningUI.Initialize(device, deviceContext, 96, 54, "Data/Textures/ui_sample.jpg", cb_vs_vertexshader_2d);
	warningUI.SetPosition(XMFLOAT3(windowwidth - 96/2, windowheight - 54/2, 0));
}

void CarSpeedUI::Update(float fillamount, float speed) {
	speedbar.UpdateFillAmount(fillamount);

	//speedUI
	speed *= 40;
	int speedf = floor(speed);
	for (size_t i = 0; i < 3; i++)
	{
		float dig = (float)(speedf % 10);
		speedf = (int)speedf / 10;
		std::vector<Vertex2D> vertexData =
		{
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 0.1 + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 0.1 + 0.1f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 0.1 + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 0.1 + 0.1f , 1.0f), //Bottom Right
		};
		speeddigf[i].UpdateUV(vertexData);
	}

	auto speedb = speed;
	for (size_t i = 0; i < 2; i++)
	{
		speedb = speedb * 10;
		float dig = (float)((int)speedb % 10);
		std::vector<Vertex2D> vertexData =
		{
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 0.1 + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 0.1 + 0.1f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 0.1 + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 0.1 + 0.1f , 1.0f), //Bottom Right
		};
		speeddigb[i].UpdateUV(vertexData);
	}

}

void CarSpeedUI::Draw(XMMATRIX orthoMatrix) {
	speedbarback.Draw(orthoMatrix);
	speedbar.Draw(orthoMatrix);

	for (size_t i = 0; i < 3; i++)
	{
		speeddigf[i].Draw(orthoMatrix);
	}
	dot.Draw(orthoMatrix);
	for (size_t i = 0; i < 2; i++)
	{
		speeddigb[i].Draw(orthoMatrix);
	}

	if (false)
	{
		warningUI.Draw(orthoMatrix);
	}
}
