#include "Fade.h"


void Fade::InitFade(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, float width, float height)
{
	fadestate = FadeState::Fade_none;
	fadesprite.Initialize(device, deviceContext, width, height, "Data/Textures/fade_black.png", cb_vs_vertexshader_2d);
	fadesprite.color = XMFLOAT4(1, 1, 1, 0);
}

void Fade::UpdateFade()
{
	
}

void Fade::DrawFade(XMMATRIX orthoMatrix)
{
	fadesprite.Draw(orthoMatrix);
}

