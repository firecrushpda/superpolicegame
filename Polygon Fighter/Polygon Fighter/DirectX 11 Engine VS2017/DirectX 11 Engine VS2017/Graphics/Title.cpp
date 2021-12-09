#include"Title.h"

void Title::Init(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d,float width,float height)
{
	Bg.Initialize(device, deviceContext, width, height, "Data/Textures/title_background.png", cb_vs_vertexshader_2d);
	//Bg.SetPosition(XMFLOAT3(0, 0, 0));
	//Bg.color = XMFLOAT4(1, 1, 1, 0.9f);
	startUI.Initialize(device, deviceContext, 400,201, "Data/Textures/title01.png", cb_vs_vertexshader_2d);
	startUI.SetPosition(XMFLOAT3(width-650, 50, 0));
	editor.Initialize(device, deviceContext, 300 / 2, 100 / 2, "Data/Textures/title_editor.png", cb_vs_vertexshader_2d);
	editor.SetPosition(XMFLOAT3(600, 450, 0));
	tutorial.Initialize(device, deviceContext, 300 / 2, 100 / 2, "Data/Textures/title_tutorial.png", cb_vs_vertexshader_2d);
	tutorial.SetPosition(XMFLOAT3(600, 500, 0));
}

void Title::Update() 
{
	//update selected bar
	
	if (alphaflag == true)
	{
		alpha -= 0.02f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
			alphaflag = false;
		}
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			alphaflag = true;
		}
	}


}

void Title::Draw(XMMATRIX orthoMatrix) {
	//Bg.Draw(orthoMatrix);
	startUI.Draw(orthoMatrix);
	//tutorial.Draw(orthoMatrix);
	//editor.Draw(orthoMatrix);
	
}
