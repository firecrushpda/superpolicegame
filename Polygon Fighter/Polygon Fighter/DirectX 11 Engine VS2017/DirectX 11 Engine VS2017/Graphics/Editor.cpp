#include "Editor.h"
#include <direct.h>


void Editor::InitializeEditor(ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader)
{
	//std::string path =  getcwd(NULL, 0);
	//StringHelper::GetFolder("D:\\", this->filename);//path + "\\Data\\Objects\\test"

	selectedGo = nullptr;

	LoadPrimitives();

	for (size_t i = 0; i < filename.size(); i++)
	{
		RenderableGameObject* go = new RenderableGameObject();
		go->Initialize(filename.at(i), device, deviceContext, cb_vs_vertexshader);
		go->SetGlobalMatirx(DirectX::XMMatrixIdentity());
		go->path = filename.at(i);
		primitives.push_back(go);
	}

	//if (primitives.size() >= 1)
	//{
	//	selectedGo = new RenderableGameObject();
	//	selectedGo = primitives.at(0);
	//}
	//else
	//{
	//	selectedGo = nullptr;
	//}
}

void Editor::LoadPrimitives() 
{
	std::ifstream inFile;
	inFile.open("Primitives.txt");

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Primitives file\n";
		return;
	}
	else {
		std::cerr << "Primitives File found\n";

		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{
				std::string fn;
				inFile >> fn;
				filename.push_back(fn);
			}
		}
	}
}

RenderableGameObject* Editor::CreatePrimitive()
{
	RenderableGameObject* go = new RenderableGameObject();
	go = primitives.at(item_current);
	
	ChangeSelectedObject(go);
	return go;
}

void Editor::ChangeSelectedObject(RenderableGameObject * cgo)
{
	this->selectedGo = cgo;
}
