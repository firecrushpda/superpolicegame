#pragma once
#include <vector>
#include "RenderableGameObject.h"
#include "..//StringHelper.h"
#include "Sprite.h"
//#include <string>

class Editor
{
public:

	int item_current = 0;

	int mapgoindex = -1;

	int cameratype = 0;

	bool mapspriteflag = true;
	Sprite mapsprite;
	float mapwidth = 600;
	float maphight = 400;

	RenderableGameObject* selectedGo;

	std::vector<std::string> filename;

	std::vector<RenderableGameObject* > primitives;

	void InitializeEditor( ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void LoadPrimitives();

	RenderableGameObject* CreatePrimitive();

	void ChangeSelectedObject(RenderableGameObject* cgo);
private:
	

	
};