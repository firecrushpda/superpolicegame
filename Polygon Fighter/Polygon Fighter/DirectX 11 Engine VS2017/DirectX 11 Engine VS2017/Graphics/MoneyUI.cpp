#include "MoneyUI.h"

void MoneyUI::Initialize(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d>& cb_vs_vertexshader_2d, float windowwidth, float windowheight, Car * car)
{
	this->car = car;
	moneypricetag.Initialize(device, deviceContext, 100, 50, "Data/Textures/moneypricetag.png", cb_vs_vertexshader_2d);
	moneypricetag.SetPosition(XMFLOAT3(windowwidth - 100, 0,0));

	for (size_t i = 0; i < 6; i++)
	{
		digf[i].Initialize(device, deviceContext, 25, 37, "Data/Textures/cartoonnum.png", cb_vs_vertexshader_2d);
		digf[i].SetPosition(windowwidth - 25 * (i+1), 50, 0);
		digf[i].SetScale(25, 37);
	}
}

void MoneyUI::Update()
{
	auto distance = car->cardistance / 3.0f;
	int money = distance * 57;
	for (size_t i = 0; i < 6; i++)
	{
		float dig = (float)(money % 10);
		money = (int)money / 10;
		std::vector<Vertex2D> vertexData =
		{
			Vertex2D(-0.5f, -0.5f, 0.0f, dig * 0.1 + 0.0f, 0.0f), //TopLeft
			Vertex2D(0.5f, -0.5f, 0.0f, dig * 0.1 + 0.1f , 0.0f), //TopRight
			Vertex2D(-0.5, 0.5, 0.0f, dig * 0.1 + 0.0f, 1.0f), //Bottom Left 
			Vertex2D(0.5f, 0.5, 0.0f, dig * 0.1 + 0.1f , 1.0f), //Bottom Right
		};
		digf[i].UpdateUV(vertexData);
	}
}

void MoneyUI::Draw(XMMATRIX orthoMatrix)
{
	if (drawflag)
	{
		moneypricetag.Draw(orthoMatrix);
		for (size_t i = 0; i < 6; i++)
		{
			digf[i].Draw(orthoMatrix);
		}
	}
	
}

void MoneyUI::Clear()
{
	car->cardistance = 0;
}

void MoneyUI::Hide()
{
	drawflag = false;
}

void MoneyUI::Show()
{
	drawflag = true;
}
