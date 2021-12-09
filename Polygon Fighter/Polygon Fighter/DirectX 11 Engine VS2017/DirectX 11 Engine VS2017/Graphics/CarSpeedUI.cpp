//=============================================================================
//
// 車スビートUI [CarSpeedUI.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "CarSpeedUI.h"

void CarSpeedUI::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d,float windowwidth,float windowheight) {

	spdigpos = XMFLOAT2(40, windowheight - 130);
	speedbarbackground.Initialize(device, deviceContext, 320, 188, "Data/Textures/tako01.png", cb_vs_vertexshader_2d);
	speedbarbackground.SetPosition(XMFLOAT3(0, windowheight - 200, 0));
	speedbar.Initialize(device, deviceContext, 240, 62, "Data/Textures/meter02.png", cb_vs_vertexshader_2d);
	speedbar.SetPosition(XMFLOAT3(spdigpos.x, spdigpos.y, 0));
	speedbarback.Initialize(device, deviceContext, 240, 62, "Data/Textures/meter01.png", cb_vs_vertexshader_2d);
	speedbarback.SetPosition(XMFLOAT3(spdigpos.x, spdigpos.y, 0));

	for (size_t i = 0; i < 3; i++)
	{
		speeddigf[2 - i].Initialize(device, deviceContext, 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
		speeddigf[2 - i].SetPosition(spdigpos.x + 85 + 26.4 * i, spdigpos.y + 40, 0);
		speeddigf[2 - i].SetScale(26.4, 40);
	}

	//dot.Initialize(device, deviceContext, 50, 50, "Data/Textures/dot.png", cb_vs_vertexshader_2d);
	//dot.SetPosition(XMFLOAT3(spdigpos.x + 55.0, spdigpos.y + 10, 0));

	for (size_t i = 0; i < 2; i++)
	{
		speeddigb[i].Initialize(device, deviceContext, 17.6, 26, "Data/Textures/score_s01.png", cb_vs_vertexshader_2d);
		speeddigb[i].SetPosition(spdigpos.x  +85 + 85 + 17.6 * i, spdigpos.y + 53, 0);
	}

	warningUI.Initialize(device, deviceContext, 320, 128, "Data/Textures/ihan01.png", cb_vs_vertexshader_2d);
	warningUI.SetPosition(XMFLOAT3(windowwidth / 2 - 160, windowheight/2 - 64, 0));

	flywarningUI.Initialize(device, deviceContext, 158, 160, "Data/Textures/tobidashi01.png", cb_vs_vertexshader_2d);
	flywarningUI.SetPosition(XMFLOAT3(windowwidth / 2 - 79 , windowheight / 2 - 80 + 190, 0));

	catchcarcountBGUI.Initialize(device, deviceContext, 192, 92, "Data/Textures/taiho01.png", cb_vs_vertexshader_2d);
	catchcarcountBGUI.SetPosition(XMFLOAT3(windowwidth - 192, 0, 0));
	catchcarcountUI.Initialize(device, deviceContext, 17.6, 26, "Data/Textures/score_s01.png", cb_vs_vertexshader_2d);
	catchcarcountUI.SetPosition(XMFLOAT3(windowwidth - 65, 35, 0));

	for (size_t i = 0; i < 2; i++)
	{
		countdown[1 - i].Initialize(device, deviceContext, 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
		countdown[1 - i].SetPosition(26.4 * i, 0, 0);
	}

}

void CarSpeedUI::Update(float fillamount, float speed,int catchcount, int cd) {
	speedbar.UpdateFillAmount(fillamount);

	//speedUI
	speed *= 200;
	int speedf = floor(speed);
	for (size_t i = 0; i < 3; i++)
	{
		float dig = (float)(speedf % 10);
		speedf = (int)speedf / 10;
		std::vector<Vertex2D> vertexData =
		{
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f/ 11.0f + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
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
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
		};
		speeddigb[i].UpdateUV(vertexData);
	}

	std::vector<Vertex2D> vertexData =
	{
		Vertex2D(-0.5f, -0.5f, 0.0f, catchcount * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
		Vertex2D(0.5f, -0.5f, 0.0f, catchcount * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
		Vertex2D(-0.5, 0.5, 0.0f, catchcount * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
		Vertex2D(0.5f, 0.5, 0.0f, catchcount * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
	};
	catchcarcountUI.UpdateUV(vertexData);

	auto countdowni = cd;
	for (size_t i = 0; i < 2; i++)
	{
		int dig = (int)(countdowni % 10);
		countdowni = (int)countdowni / 10;
		std::vector<Vertex2D> vertexData =
		{
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
		};
		countdown[i].UpdateUV(vertexData);
	}
	
}

void CarSpeedUI::Draw(XMMATRIX orthoMatrix, bool warningUIflag, bool flywarningUIflag,bool contdownflag) {
	speedbarbackground.Draw(orthoMatrix);
	speedbarback.Draw(orthoMatrix);
	speedbar.Draw(orthoMatrix);

	for (size_t i = 0; i < 3; i++)
		speeddigf[i].Draw(orthoMatrix);

	//dot.Draw(orthoMatrix);
	for (size_t i = 0; i < 2; i++)
		speeddigb[i].Draw(orthoMatrix);

	if (warningUIflag)
		warningUI.Draw(orthoMatrix);

	if (flywarningUIflag)
		flywarningUI.Draw(orthoMatrix);

	catchcarcountBGUI.Draw(orthoMatrix);
	catchcarcountUI.Draw(orthoMatrix);

	if (contdownflag)
	{
		for (size_t i = 0; i < 2; i++) {
			countdown[i].Draw(orthoMatrix);
		}
	}
	

}
