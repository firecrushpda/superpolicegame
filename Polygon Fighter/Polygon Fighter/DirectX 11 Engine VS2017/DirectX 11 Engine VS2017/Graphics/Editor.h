#pragma once
#include <vector>
#include "RenderableGameObject.h"
#include "..//StringHelper.h"
//#include <string>

class Editor
{
public:

	int item_current = 0;

	int mapgoindex = -1;

	RenderableGameObject* selectedGo;

	std::vector<std::string> filename;

	std::vector<RenderableGameObject* > primitives;

	void InitializeEditor( ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader);
	void LoadPrimitives();

	RenderableGameObject* CreatePrimitive();

	void ChangeSelectedObject(RenderableGameObject* cgo);
private:
	

	
};