//=============================================================================
//
// グラフィックス[Graphics.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Graphics.h"


//=============================================================================
// 初期化
//=============================================================================
bool Graphics::Initialize(HWND hwnd, int width, int height)
{
	this->windowWidth = width;
	this->windowHeight = height;
	this->fpsTimer.Start();//タイマー

	//DirectX設置
	if (!InitializeDirectX(hwnd))
		return false;

	//
	if (!InitalizeBuffers())
		return false;

	//シェーダー設置
	if (!InitializeShaders())
		return false;

	//ゲームシーン初期化
	if (!InitializeScene())
		return false;

	//IBL試して
	if (!InitializeIBLStatus())
		return false;
	

	//effekseer 
	/*if (!InitializeEffekseer())
		return false;*/

	//ImGui (デイバッグサポートUI)初期化
	//Setup ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	return true;
}

//=============================================================================
// 描画
//=============================================================================
void Graphics::RenderFrame()
{
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));
	Camera3D.viewport = viewport;
	this->deviceContext->RSSetViewports(1, &viewport);
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//バックグラウンド色
	float bgcolor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), bgcolor);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	deviceContext->OMSetBlendState(blendState.Get(), NULL, 0xFFFFFFFF);

	deviceContext->IASetInputLayout(vertexshader_skyBox.GetInputLayout());
	deviceContext->VSSetShader(vertexshader_skyBox.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader_skyBox.GetShader(), NULL, 0);
	deviceContext->RSSetState(rasterizerState_CullFront.Get());
	deviceContext->OMSetDepthStencilState(depthUnenableStencilState.Get(), 0);
	//スカイボックス描画
	skybox.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());

	deviceContext->VSSetShader(vertexshader.GetShader(), NULL, 0);
	deviceContext->PSSetShader(pixelshader.GetShader(), NULL, 0);

	//ライト設置
	//cb_ps_light.data.dynamicLightColor = light.lightColor;
	//cb_ps_light.data.dynamicLightStrength = light.lightStrength;
	//cb_ps_light.data.dynamicLightPosition = light.GetPositionFloat3();
	//cb_ps_light.data.dynamicLightAttenuation_a = light.attenuation_a;
	//cb_ps_light.data.dynamicLightAttenuation_b = light.attenuation_b;
	//cb_ps_light.data.dynamicLightAttenuation_c = light.attenuation_c;
	//cb_ps_light.ApplyChanges();

	//directional light
	light.SetLookAtPos(XMFLOAT3(0.0f, 0.0f, 0.0f));
	cb_ps_light.data.directionalLight.colour = light.lightColor;
	cb_ps_light.data.directionalLight.ambientStrength = light.lightStrength;
	XMStoreFloat3(&cb_ps_light.data.directionalLight.direction, light.GetForwardVector());

	//templary set PointLights and SpotLights to 0
	cb_ps_light.data.numPointLights = 1;
	cb_ps_light.data.numSpotLights = 0;
	//spot light
	/*cb_ps_light.data.spotLights[0].colour = spotLights.at(0)->lightColor;
	cb_ps_light.data.spotLights[0].attenuationConstant = spotLights.at(0)->attenuationConstant;
	cb_ps_light.data.spotLights[0].attenuationLinear = spotLights.at(0)->attenuationLinear;
	cb_ps_light.data.spotLights[0].attenuationQuadratic = spotLights.at(0)->attenuationQuadratic;
	XMStoreFloat3(&cb_ps_light.data.spotLights[0].direction, spotLights.at(0)->GetForwardVector());
	cb_ps_light.data.spotLights[0].innerCutoff = spotLights.at(0)->innerCutoff;
	cb_ps_light.data.spotLights[0].outerCutoff = spotLights.at(0)->outerCutoff;
	cb_ps_light.data.spotLights[0].position = spotLights.at(0)->GetPositionFloat3();*/

	//point light
	cb_ps_light.data.pointLights[0].colour = pointLights.at(0)->lightColor;
	cb_ps_light.data.pointLights[0].position = pointLights.at(0)->GetPositionFloat3();
	cb_ps_light.data.pointLights[0].strength = pointLights.at(0)->lightStrength;
	cb_ps_light.data.pointLights[0].radius = pointLights.at(0)->radius;
	cb_ps_light.data.cameraPosition = Camera3D.GetPositionFloat3();
	cb_ps_light.ApplyChanges();
	deviceContext->PSSetConstantBuffers(0, 1, cb_ps_light.GetAddressOf());

	//material
	//cb_ps_iblstatus.data.roughness = 1.0f;
	//cb_ps_iblstatus.data.metallic = 1.0f;
	cb_ps_iblstatus.data.color = XMFLOAT4(1, 1, 1, 1);
	cb_ps_iblstatus.ApplyChanges();
	deviceContext->PSSetConstantBuffers(1, 1, this->cb_ps_iblstatus.GetAddressOf());

	deviceContext->RSSetState(rasterizerState.Get());
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	deviceContext->IASetInputLayout(vertexshader.GetInputLayout());
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	{ 
		//シェーダーリソース
		this->deviceContext->PSSetShaderResources(4, 1, &brdfLUTSRV);
		this->deviceContext->PSSetShaderResources(5, 1, &skyIBLSRV);
		this->deviceContext->PSSetShaderResources(6, 1, &envMapSRV);
		
		if (gs == GameState::game)
		{
			car.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			chasecar.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());

			//ステージ描画
			//stage.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
		}

		if (gs == GameState::editor)
		{
			if (m_editor.selectedGo != nullptr)
			{
				cb_ps_iblstatus.data.color = XMFLOAT4(0, 0.8, 0.8, 0.8);
				cb_ps_iblstatus.ApplyChanges();

				m_editor.selectedGo->Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			}

			cb_ps_iblstatus.data.color = XMFLOAT4(1,1,1,1);
			cb_ps_iblstatus.ApplyChanges();
			for (size_t i = 0; i < mapgo.size(); i++)
			{
				if (mapgo.at(i) == m_editor.selectedGo)
					continue;

				mapgo.at(i)->Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			} 
		}

	}
	{
		//ライト描画
		deviceContext->PSSetShader(pixelshader_nolight.GetShader(), NULL, 0);
		//light.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());

		//EffekseerDraw();
		
	}

	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(vertexshader_2d.GetInputLayout());
	deviceContext->PSSetShader(pixelshader_2d.GetShader(), NULL, 0);
	deviceContext->VSSetShader(vertexshader_2d.GetShader(), NULL, 0);
	if (gs == GameState::title)
	{
		title.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
	}

	if (gs == GameState::game)
	{
		car.carsui.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		if (car.warninguiflag)
		{
			car.warningui.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		}
	}

	if (gs == GameState::tutorial)
	{
		tutorial.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
	}

	if (gs == GameState::score)
	{
		score.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
	}
	
	fade.DrawFade(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

	//Draw Text
	//タイマー表示
	static int fpsCounter = 0;
	static std::string fpsString = "FPS: 0";
	fpsCounter += 1;
	if (fpsTimer.GetMilisecondsElapsed() > 1000.0)
	{
		fpsString = "FPS: " + std::to_string(fpsCounter);
		fpsCounter = 0;
		fpsTimer.Restart();
	}

	if (gs == GameState::game)
	{
		//car pos debug text
		auto carpos = car.carrender.GetPositionFloat3();
		std::string pos = std::to_string(carpos.x) + "_" + std::to_string(carpos.y) + "_" + std::to_string(carpos.z);
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
		std::wstring wsnum = cv.from_bytes(pos);

		auto vel = car.GetCarVelocity();
		std::string veltext = std::to_string(vel);
		std::wstring velutf8 = cv.from_bytes(veltext);

		//collision text debug
		auto cocar = car.carrender.GetCollisionObject();
		auto cochasecar = chasecar.carrender.GetCollisionObject();
		auto cocamera = Camera3D.GetCameraCollision();

		DirectX::ContainmentType coresult = cocamera->frustum.Contains(cochasecar->obb);
		bool testbool = false;
		coresult == 0 ? testbool = true : testbool = false;
		std::string testbooltext = std::to_string(coresult);
		std::wstring testboolutf8 = cv.from_bytes(testbooltext);

		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteFont->DrawString(spriteBatch.get(), wsnum.c_str(), DirectX::XMFLOAT2(0, 20), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteFont->DrawString(spriteBatch.get(), velutf8.c_str(), DirectX::XMFLOAT2(0, 40), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteFont->DrawString(spriteBatch.get(), testboolutf8.c_str(), DirectX::XMFLOAT2(0, 60), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteBatch->End();
	}
	if (gs == GameState::editor)
	{
		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteBatch->End();
	}
	
	//サポートUI描画
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (showImgui && gs == GameState::game)
	{
		//Create ImGui Test Window
		ImGui::Begin("Light Controls");
		//ImGui::NewLine();
		//ImGui::DragFloat3("Ambient Light Color", &this->cb_ps_light.data.ambientLightColor.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Metallic", &this->cb_ps_iblstatus.data.metallic, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Roughness", &this->cb_ps_iblstatus.data.roughness, 0.01f, 0.0f, 1.0f);
		//ImGui::DragFloat("Ambient Light shininess", &this->cb_ps_light.data.objectMaterial.shininess, 0.01f, 0.0f, 10.0f);
		//ImGui::DragFloat("Ambient Light specularity", &this->cb_ps_light.data.objectMaterial.specularity, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat3("Ambient Light Color", &this->light.lightColor.x, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Ambient Light Strength", &this->light.lightStrength, 0.01f, 0.0f, 10.0f);
		ImGui::NewLine();
		ImGui::DragFloat3("Dynamic Light Color", &this->pointLights.at(0)->lightColor.x, 0.01f, 0.0f, 1.0f);
		auto pointlightpos = pointLights.at(0)->GetPositionFloat3();
		ImGui::DragFloat3("Dynamic Light Strength", &pointlightpos.x, 1.0f, -100.0f, 100.0f);
		pointLights.at(0)->SetPosition(pointlightpos);

		ImGui::End();
	}

	if (gs == GameState::editor && showImgui == true)
	{
		ImGui::Begin("Editor");

		//fucking vector<string> to char*[]
		std::vector<char*> clist;
		for (size_t i = 0; i < m_editor.filename.size(); i++)
		{
			clist.push_back(m_editor.filename.at(i).data());
		}
		ImGui::Combo("combo", &m_editor.item_current, &clist[0], m_editor.filename.size());

		if (ImGui::Button("Add to Scene")) {
			RenderableGameObject* rgo = new RenderableGameObject();
			rgo = m_editor.primitives.at(m_editor.item_current);
			m_editor.selectedGo = rgo;
			mapgo.push_back(rgo);
		}
			
		if (ImGui::Button("Delete from Scene"))
		{
			//delete m_editor.selectedGo;
			if (m_editor.selectedGo != nullptr)
			{
				m_editor.selectedGo = nullptr;
				if (m_editor.mapgoindex != -1)
					mapgo.erase(mapgo.begin() + m_editor.mapgoindex);
			}
		}
		
		if (ImGui::Button("Save"))
		{
			ofstream out("Map.txt");
			out.clear();
			for (size_t i = 0; i < mapgo.size(); i++)
			{
				auto pos = mapgo.at(i)->GetPositionFloat3();
				auto rot = mapgo.at(i)->GetRotationFloat3();
				auto scl = mapgo.at(i)->GetScaleFloat3();
				out << "O" << " " << mapgo.at(i)->path + "\n";
				out << pos.x << " " << pos.y << " " << pos.z << "\n";
				out << rot.x << " " << rot.y << " " << rot.z << "\n";
				out << scl.x << " " << scl.y << " " << scl.z << "\n";
			}
			out.close();
		}

		//Create ImGui Test Window
		if (m_editor.selectedGo != nullptr)
		{
			ImGui::NewLine();
			auto spos = m_editor.selectedGo->GetPositionFloat3();
			ImGui::DragFloat3("Position", &spos.x, 1.0f, -10000.0f, 10000.0f);
			m_editor.selectedGo->SetPosition(spos);

			auto srot = m_editor.selectedGo->GetRotationFloat3();
			srot = XMFLOAT3(srot.x / XM_PI * 180, srot.y / XM_PI * 180, srot.z / XM_PI * 180);
			ImGui::DragFloat3("Rotation", &srot.x, 1.0f, -180.0f, 180.0f);
			srot = XMFLOAT3(srot.x * XM_PI / 180, srot.y * XM_PI / 180, srot.z * XM_PI / 180);
			m_editor.selectedGo->SetRotation(srot);

			auto sscl = m_editor.selectedGo->GetScaleFloat3();
			ImGui::DragFloat3("Scale", &sscl.x, 0.1f, -10000.0f, 10000.0f);
			m_editor.selectedGo->SetScale(sscl);
			
		}
		ImGui::End();
	}

	//Assemble Together Draw Data
	ImGui::Render();
	//Render Draw Data
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	this->swapchain->Present(0, NULL);
}

//=============================================================================
//DirectX設置
//=============================================================================
bool Graphics::InitializeDirectX(HWND hwnd)
{
	try
	{
		std::vector<AdapterData> adapters = AdapterReader::GetAdapters();

		if (adapters.size() < 1)
		{
			ErrorLogger::Log("No IDXGI Adapters found.");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC scd = { 0 };

		scd.BufferDesc.Width = this->windowWidth;
		scd.BufferDesc.Height = this->windowHeight;
		scd.BufferDesc.RefreshRate.Numerator = 60;
		scd.BufferDesc.RefreshRate.Denominator = 1;
		scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


		scd.SampleDesc.Count = 1;//4
		scd.SampleDesc.Quality = 0;//m_4xMsaaQuality - 1

		scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scd.BufferCount = 1;
		scd.OutputWindow = hwnd;
		scd.Windowed = TRUE;
		scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr;
		hr = D3D11CreateDeviceAndSwapChain(adapters[0].pAdapter, //IDXGI Adapter
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL, //FOR SOFTWARE DRIVER TYPE
			NULL, //FLAGS FOR RUNTIME LAYERS
			NULL, //FEATURE LEVELS ARRAY
			0, //# OF FEATURE LEVELS IN ARRAY
			D3D11_SDK_VERSION,
			&scd, //Swapchain description
			this->swapchain.GetAddressOf(), //Swapchain Address
			this->device.GetAddressOf(), //Device Address
			NULL, //Supported feature level
			this->deviceContext.GetAddressOf()); //Device Context Address

		COM_ERROR_IF_FAILED(hr, "Failed to create device and swapchain.");

		Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
		hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
		COM_ERROR_IF_FAILED(hr, "GetBuffer Failed.");

		hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create render target view.");

		//Describe our Depth/Stencil Buffer
		CD3D11_TEXTURE2D_DESC depthStencilTextureDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, this->windowWidth, this->windowHeight);
		depthStencilTextureDesc.MipLevels = 1;
		depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = this->device->CreateTexture2D(&depthStencilTextureDesc, NULL, this->depthStencilBuffer.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil buffer.");

		hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil view.");

		this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

		//Create depth stencil state
		CD3D11_DEPTH_STENCIL_DESC depthstencildesc(D3D11_DEFAULT);
		depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		//depthstencildesc.DepthEnable = false;
		
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");

		depthstencildesc.DepthEnable = true;
		depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthUnenableStencilState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create depth stencil state.");
		

		//Create & set the Viewport
		CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight));;
		this->deviceContext->RSSetViewports(1, &viewport);

		//Create Rasterizer State
		CD3D11_RASTERIZER_DESC rasterizerDesc(D3D11_DEFAULT);
		rasterizerDesc.DepthClipEnable = false;
		hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Rasterizer State for culling front
		CD3D11_RASTERIZER_DESC rasterizerDesc_CullFront(D3D11_DEFAULT);
		rasterizerDesc_CullFront.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc_CullFront.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		hr = this->device->CreateRasterizerState(&rasterizerDesc_CullFront, this->rasterizerState_CullFront.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create rasterizer state.");

		//Create Blend State
		D3D11_RENDER_TARGET_BLEND_DESC rtbd = { 0 };
		rtbd.BlendEnable = true;
		rtbd.SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;//D3D11_BLEND_SRC_COLOR D3D11_BLEND_SRC_ALPHA
		rtbd.DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;//D3D11_BLEND_INV_SRC_ALPHA
		rtbd.BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		rtbd.DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		rtbd.BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		D3D11_BLEND_DESC blendDesc = { 0 };
		blendDesc.RenderTarget[0] = rtbd;
		blendDesc.AlphaToCoverageEnable = true;

		hr = this->device->CreateBlendState(&blendDesc, this->blendState.GetAddressOf());
		COM_ERROR_IF_FAILED(hr, "Failed to create blend state.");

		spriteBatch = std::make_unique<DirectX::SpriteBatch>(this->deviceContext.Get());
		spriteFont = std::make_unique<DirectX::SpriteFont>(this->device.Get(), L"Data\\Fonts\\comic_sans_ms_16.spritefont");

		//Create sampler description for sampler state
		CD3D11_SAMPLER_DESC sampDesc(D3D11_DEFAULT);
		sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;//D3D11_FILTER_MIN_MAG_MIP_LINEAR D3D11_FILTER_ANISOTROPIC
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MaxAnisotropy = 16;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = this->device->CreateSamplerState(&sampDesc, this->samplerState.GetAddressOf()); //Create sampler state
		COM_ERROR_IF_FAILED(hr, "Failed to create sampler state.");

	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

//=============================================================================
//シェーダー初期化
//=============================================================================
bool Graphics::InitializeShaders()
{

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG //Debug Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Debug\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Debug\\";
	#endif
	#else //Release Mode
	#ifdef _WIN64 //x64
			shaderfolder = L"..\\x64\\Release\\";
	#else  //x86 (Win32)
			shaderfolder = L"..\\Release\\";
	#endif
#endif
	}

	//2d shaders
	//2dシェーダーレイアウト
	D3D11_INPUT_ELEMENT_DESC layout2D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		//{ "COLOR",   0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements2D = ARRAYSIZE(layout2D);

	//シェーダー初期化
	if (!vertexshader_2d.Initialize(this->device, shaderfolder + L"vertexshader_2d.cso", layout2D, numElements2D))
		return false;

	if (!pixelshader_2d.Initialize(this->device, shaderfolder + L"pixelshader_2d.cso"))
		return false;

	//3d shaders
	//3dシェーダーレイアウト
	D3D11_INPUT_ELEMENT_DESC layout3D[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		//{"BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 1, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElements3D = ARRAYSIZE(layout3D);

	//シェーダー初期化
	if (!vertexshader.Initialize(this->device, shaderfolder + L"vertexshader.cso", layout3D, numElements3D))
		return false;

	if (!pixelshader.Initialize(this->device, shaderfolder + L"pixelshader.cso"))
		return false;

	if (!pixelshader_nolight.Initialize(this->device, shaderfolder + L"pixelshader_nolight.cso"))
		return false;

	//skybox
	//skyboxシェーダーレイアウト
	D3D11_INPUT_ELEMENT_DESC layoutskyBox[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};

	UINT numElementsskyBox = ARRAYSIZE(layoutskyBox);

	//シェーダー初期化
	if (!vertexshader_skyBox.Initialize(this->device, shaderfolder + L"vertexshader_skyBox.cso", layoutskyBox, numElementsskyBox))
		return false;

	if (!pixelshader_skyBox.Initialize(this->device, shaderfolder + L"pixelshader_skyBox.cso"))
		return false;
	
	if (!ConvolutionPixelShader.Initialize(this->device, shaderfolder + L"ConvolutionPixelShader.cso"))
		return false;
	
	if (!PrefilterMapPixelShader.Initialize(this->device, shaderfolder + L"PrefilterMapPixelShader.cso"))
		return false;

	if (!IntegrateBRDFPixelShader.Initialize(this->device, shaderfolder + L"IntegrateBRDFPixelShader.cso"))
		return false;

	return true;
}

//=============================================================================
//ゲームシーン初期化
//=============================================================================
bool Graphics::InitializeScene()
{
	try
	{
		//title
		title.Init(this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader_2d, windowWidth, windowHeight);
		fade.InitFade(this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader_2d, windowWidth, windowHeight);

		//game state
		gs = GameState::title;
		tempgs = GameState::title;

		//tutorial
		tutorial.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight,"Data\\Textures\\tutoial.png" ,this->cb_vs_vertexshader_2d);

		//score
		score.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\result.jpg", this->cb_vs_vertexshader_2d);

		if(!car.CarInitialize("Data\\Objects\\test\\police.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;// bill.obj p.obj taxi\\testtaxi.obj p.obj jk_bread.fbx
		car.carrender.SetScale(3, 3, 3);
		car.carrender.SetCollisionBoxView(true);

		if (!chasecar.CarInitialize("Data\\Objects\\test\\police.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
		chasecar.carrender.SetScale(5.0f, 5.0f, 5.0f);
		chasecar.carrender.SetCollisionBoxView(true);
		
		cac = new CarAIController();
		cac->SetAICar(&chasecar);
		auto carpos = car.carrender.GetPositionFloat3();
		cac->SetPLayerCarPos(&carpos);

		//ゲームステージ初期化
		//if (!stage.Initialize("Data\\Objects\\Stage.FBX", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
		//	return false;
		//
		//auto stageco = stage.GetCollisionObject();
		//stageco->collisionuse = false;
		//stage.SetCollisionBoxView(false);

		LoadMap();

		if (!quad.Initialize("Data\\Objects\\quad.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;//
		auto quadco = quad.GetCollisionObject();
		quadco->collisionuse = false;
		quad.SetCollisionBoxView(false);

		//スカイボックス初期化
		if (!skybox.Initialize("Data\\Objects\\skybox.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;

		//レイト初期化
		if (!light.Initialize(this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
		light.SetLookAtPos(XMFLOAT3(0, 0, 0));
		light.SetPosition(XMFLOAT3(0, 10, 0.0f));
		light.lightColor = XMFLOAT3(1.0f, 1.0, 1.0);

		//SpotLight* spotLight = new SpotLight();
		//spotLight->SetPosition(XMFLOAT3(0,0,-1000));
		////spotLight->lightColor = XMFLOAT3(0, 0, 0);
		//spotLights.push_back(spotLight);

		PointLight* pointlight = new PointLight();
		pointlight->SetPosition(0, 0, -100);
		pointLights.push_back(pointlight);

		//スプライト初期化
		car.carsui.Initialize(this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader_2d);
		car.warningui.Initialize(this->device.Get(), this->deviceContext.Get(),96,37, "Data\\Textures\\warningtag.png", cb_vs_vertexshader_2d);
		car.warningui.SetPosition(XMFLOAT3(windowWidth / 2 - 48, windowHeight / 2 - 19, 0));

		//カメラ設置
		camera2D.SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

		//camera3D
		Camera3D.ChangeFocusMode(0, &car.carrender);
		Camera3D.SetProjectionValues(90, static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 3000.0f);
	}
	catch (COMException & exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

bool Graphics::InitalizeBuffers() {
	//Initialize Constant Buffer(s)
	//定数バッファ初期化
	HRESULT hr = this->cb_vs_vertexshader_2d.Initialize(this->device.Get(), this->deviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize 2d constant buffer.");

	hr = this->cb_vs_vertexshader.Initialize(this->device.Get(), this->deviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize vertexshader constant buffer.");

	hr = this->cb_ps_light.Initialize(this->device.Get(), this->deviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize light constant buffer.");
	cb_ps_light.data.useNormalMapping = true;
	cb_ps_light.data.useParallaxOcclusionMapping = true;
	cb_ps_light.data.objectMaterial.shininess = 8.0f;
	cb_ps_light.data.objectMaterial.specularity = 0.75f;

	hr = this->cb_ps_iblstatus.Initialize(this->device.Get(), this->deviceContext.Get());
	COM_ERROR_IF_FAILED(hr, "Failed to initialize light constant buffer.");
	cb_ps_iblstatus.data.metallic = 0.0f;
	cb_ps_iblstatus.data.roughness = 0.0f;
	cb_ps_iblstatus.data.color = XMFLOAT4(1, 1, 1, 1);

	return true;
}

bool Graphics::InitializeIBLStatus()
{
	auto start = std::chrono::high_resolution_clock::now();

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(vertexshader_skyBox.GetInputLayout());

	XMMATRIX m_GlobalInverseTransform = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, -1, 0, 0,
		0, 0, 0, 1
	);

	XMFLOAT3 position = XMFLOAT3(0, 0, 0);
	XMMATRIX camViewMatrix = Camera3D.GetViewMatrix();
	XMMATRIX camProjMatrix = Camera3D.GetProjectionMatrix();
	XMVECTOR tar[] = { XMVectorSet(1, 0, 0, 0), XMVectorSet(-1, 0, 0, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, -1, 0, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 0, -1, 0) };
	XMVECTOR up[] = { XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 0, -1, 0), XMVectorSet(0, 0, 1, 0), XMVectorSet(0, 1, 0, 0), XMVectorSet(0, 1, 0, 0) };
	//---
	UINT stride = sizeof(Vertex3D_SkyBox);
	UINT offset = 0;
	const float color[4] = { 0.6f, 0.6f, 0.6f, 0.0f };
#pragma region Diffuse IBL
	// DIFFUSE IBL CONVOLUTION

	D3D11_TEXTURE2D_DESC skyIBLDesc;
	ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	skyIBLDesc.Width = 64;
	skyIBLDesc.Height = 64;
	skyIBLDesc.MipLevels = 1;
	skyIBLDesc.ArraySize = 6;
	skyIBLDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	skyIBLDesc.Usage = D3D11_USAGE_DEFAULT;
	skyIBLDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	skyIBLDesc.CPUAccessFlags = 0;
	skyIBLDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	skyIBLDesc.SampleDesc.Count = 1;
	skyIBLDesc.SampleDesc.Quality = 0;
	//---
	ID3D11RenderTargetView* skyIBLRTV[6];
	//--
	D3D11_RENDER_TARGET_VIEW_DESC skyIBLRTVDesc;
	ZeroMemory(&skyIBLRTVDesc, sizeof(skyIBLRTVDesc));
	skyIBLRTVDesc.Format = skyIBLDesc.Format;
	skyIBLRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	skyIBLRTVDesc.Texture2DArray.ArraySize = 1;
	skyIBLRTVDesc.Texture2DArray.MipSlice = 0;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC skyIBLSRVDesc;
	ZeroMemory(&skyIBLSRVDesc, sizeof(skyIBLSRVDesc));
	skyIBLSRVDesc.Format = skyIBLDesc.Format;
	skyIBLSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	skyIBLSRVDesc.TextureCube.MostDetailedMip = 0;
	skyIBLSRVDesc.TextureCube.MipLevels = 1;
	//---
	D3D11_VIEWPORT skyIBLviewport;
	skyIBLviewport.Width = 64;
	skyIBLviewport.Height = 64;
	skyIBLviewport.MinDepth = 0.0f;
	skyIBLviewport.MaxDepth = 1.0f;
	skyIBLviewport.TopLeftX = 0.0f;
	skyIBLviewport.TopLeftY = 0.0f;
	//---

	device->CreateTexture2D(&skyIBLDesc, 0, &skyIBLtex);
	device->CreateShaderResourceView(skyIBLtex, &skyIBLSRVDesc, &skyIBLSRV);
	//context->GenerateMips(skyIBLSRV);
	for (int i = 0; i < 6; i++) {
		skyIBLRTVDesc.Texture2DArray.FirstArraySlice = i;
		device->CreateRenderTargetView(skyIBLtex, &skyIBLRTVDesc, &skyIBLRTV[i]);
		//-- Cam directions
		XMVECTOR dir = XMVector3Rotate(tar[i], XMQuaternionIdentity());
		XMMATRIX view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&position), dir, up[i]);
		//XMStoreFloat4x4(&camViewMatrix, DirectX::XMMatrixTranspose(view));

		XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.0f, 0.1f, 100.0f);
		//XMStoreFloat4x4(&camProjMatrix, DirectX::XMMatrixTranspose(P));


		deviceContext->OMSetRenderTargets(1, &skyIBLRTV[i], 0);
		deviceContext->RSSetViewports(1, &skyIBLviewport);
		deviceContext->ClearRenderTargetView(skyIBLRTV[i], color);

		auto vertexBuffer = skybox.vertexbuffer.GetAddressOf();
		auto indexBuffer = skybox.indexbuffer.Get();

		deviceContext->VSSetShader(vertexshader_skyBox.GetShader(), 0, 0);
		deviceContext->PSSetShader(ConvolutionPixelShader.GetShader(), 0, 0);

		this->cb_vs_vertexshader.data.wvpMatrix = DirectX::XMMatrixIdentity() * view * P;//m_GlobalInverseTransform
		this->cb_vs_vertexshader.data.worldMatrix =  DirectX::XMMatrixIdentity();//m_GlobalInverseTransform
		this->cb_vs_vertexshader.ApplyChanges();
		deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());

		this->deviceContext->PSSetShaderResources(0, 1, skybox.textureView.GetAddressOf());
		deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
		deviceContext->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
		deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->RSSetState(rasterizerState_CullFront.Get());
		deviceContext->OMSetDepthStencilState(depthUnenableStencilState.Get(), 0);
		//deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

		deviceContext->DrawIndexed(skybox.indices.size(), 0, 0);

		// Reset the render states we've changed
		/*deviceContext->RSSetState(0);
		deviceContext->OMSetDepthStencilState(0, 0);*/

	}

	for (int i = 0; i < 6; i++) {
		skyIBLRTV[i]->Release();
	}

#pragma endregion
#pragma region Prefilter EnvMap
	// PREFILTER ENVIRONMENT MAP
	unsigned int maxMipLevels = 5;
	D3D11_TEXTURE2D_DESC envMapDesc;
	//ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	envMapDesc.Width = 256;
	envMapDesc.Height = 256;
	envMapDesc.MipLevels = maxMipLevels;
	envMapDesc.ArraySize = 6;
	envMapDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	envMapDesc.Usage = D3D11_USAGE_DEFAULT;
	envMapDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	envMapDesc.CPUAccessFlags = 0;
	envMapDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE | D3D11_RESOURCE_MISC_GENERATE_MIPS;
	envMapDesc.SampleDesc.Count = 1;
	envMapDesc.SampleDesc.Quality = 0;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC envMapSRVDesc;
	ZeroMemory(&envMapSRVDesc, sizeof(envMapSRVDesc));
	envMapSRVDesc.Format = skyIBLDesc.Format;
	envMapSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	envMapSRVDesc.TextureCube.MostDetailedMip = 0;
	envMapSRVDesc.TextureCube.MipLevels = maxMipLevels;
	//--
	ID3D11RenderTargetView* envMapRTV[6];
	//---
	device->CreateTexture2D(&envMapDesc, 0, &envMaptex);
	device->CreateShaderResourceView(envMaptex, &envMapSRVDesc, &envMapSRV);
	for (int mip = 0; mip < maxMipLevels; mip++) {

		D3D11_RENDER_TARGET_VIEW_DESC envMapRTVDesc;
		ZeroMemory(&envMapRTVDesc, sizeof(envMapRTVDesc));
		envMapRTVDesc.Format = skyIBLDesc.Format;
		envMapRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		envMapRTVDesc.Texture2DArray.ArraySize = 1;
		envMapRTVDesc.Texture2DArray.MipSlice = mip;

		unsigned mipWidth = 256 * pow(0.5, mip);
		unsigned mipHeight = 256 * pow(0.5, mip);

		D3D11_VIEWPORT envMapviewport;
		envMapviewport.Width = mipWidth;
		envMapviewport.Height = mipHeight;
		envMapviewport.MinDepth = 0.0f;
		envMapviewport.MaxDepth = 1.0f;
		envMapviewport.TopLeftX = 0.0f;
		envMapviewport.TopLeftY = 0.0f;


		float roughness = (float)mip / (float)(maxMipLevels - 1);
		//float roughness = 0.0;
		for (int i = 0; i < 6; i++) {
			envMapRTVDesc.Texture2DArray.FirstArraySlice = i;
			device->CreateRenderTargetView(envMaptex, &envMapRTVDesc, &envMapRTV[i]);

			//---
			deviceContext->OMSetRenderTargets(1, &envMapRTV[i], 0);
			deviceContext->RSSetViewports(1, &envMapviewport);
			deviceContext->ClearRenderTargetView(envMapRTV[i], color);
			//---

			auto vertexBuffer = skybox.vertexbuffer.GetAddressOf();
			auto indexBuffer = skybox.indexbuffer.Get();

			deviceContext->VSSetShader(vertexshader_skyBox.GetShader(), 0, 0);
			deviceContext->PSSetShader(PrefilterMapPixelShader.GetShader(), 0, 0);

			XMVECTOR dir = XMVector3Rotate(tar[i], XMQuaternionIdentity());
			XMMATRIX view = DirectX::XMMatrixLookToLH(XMLoadFloat3(&position), dir, up[i]);
			//XMStoreFloat4x4(&camViewMatrix, DirectX::XMMatrixTranspose(view));

			XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.5f * XM_PI, 1.0f, 0.1f, 100.0f);
			//XMStoreFloat4x4(&camProjMatrix, DirectX::XMMatrixTranspose(P));

			this->cb_vs_vertexshader.data.wvpMatrix =   DirectX::XMMatrixIdentity() * view * P;//m_GlobalInverseTransform
			this->cb_vs_vertexshader.data.worldMatrix =   DirectX::XMMatrixIdentity();//m_GlobalInverseTransform
			this->cb_vs_vertexshader.ApplyChanges();
			deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());

			cb_ps_iblstatus.data.roughness = roughness;
			cb_ps_iblstatus.ApplyChanges();
			deviceContext->PSSetConstantBuffers(0, 1, this->cb_ps_iblstatus.GetAddressOf());

			deviceContext->IASetVertexBuffers(0, 1, vertexBuffer, &stride, &offset);
			deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

			deviceContext->PSSetShaderResources(0, 1, skybox.textureView.GetAddressOf());
			deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
			//PrefilterMapPixelShader->SetShaderResourceView("EnvMap", skySRV);

			deviceContext->RSSetState(rasterizerState_CullFront.Get());
			deviceContext->OMSetDepthStencilState(depthUnenableStencilState.Get(), 0);

			deviceContext->DrawIndexed(skybox.indices.size(), 0, 0);

			// Reset the render states we've changed
			/*deviceContext->RSSetState(0);
			deviceContext->OMSetDepthStencilState(0, 0);*/

		}
		for (int i = 0; i < 6; i++) {
			envMapRTV[i]->Release();
		}

	}
#pragma endregion

#pragma region Integrate BRDF LUT
	// INTEGRATE BRDF & CREATE LUT

	D3D11_TEXTURE2D_DESC brdfLUTDesc;
	//ZeroMemory(&skyIBLDesc, sizeof(skyIBLDesc));
	brdfLUTDesc.Width = 512;
	brdfLUTDesc.Height = 512;
	brdfLUTDesc.MipLevels = 0;
	brdfLUTDesc.ArraySize = 1;
	brdfLUTDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	brdfLUTDesc.Usage = D3D11_USAGE_DEFAULT;
	brdfLUTDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	brdfLUTDesc.CPUAccessFlags = 0;
	brdfLUTDesc.MiscFlags = 0;
	brdfLUTDesc.SampleDesc.Count = 1;
	brdfLUTDesc.SampleDesc.Quality = 0;
	//---
	ID3D11RenderTargetView* brdfLUTRTV;
	//--
	D3D11_RENDER_TARGET_VIEW_DESC brdfLUTRTVDesc;
	ZeroMemory(&brdfLUTRTVDesc, sizeof(brdfLUTRTVDesc));
	brdfLUTRTVDesc.Format = brdfLUTDesc.Format;
	brdfLUTRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//---
	D3D11_SHADER_RESOURCE_VIEW_DESC brdfLUTSRVDesc;
	ZeroMemory(&brdfLUTSRVDesc, sizeof(brdfLUTSRVDesc));
	brdfLUTSRVDesc.Format = brdfLUTSRVDesc.Format;
	brdfLUTSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	brdfLUTSRVDesc.TextureCube.MostDetailedMip = 0;
	brdfLUTSRVDesc.TextureCube.MipLevels = 1;
	//---
	D3D11_VIEWPORT brdfLUTviewport;
	brdfLUTviewport.Width = 512;
	brdfLUTviewport.Height = 512;
	brdfLUTviewport.MinDepth = 0.0f;
	brdfLUTviewport.MaxDepth = 1.0f;
	brdfLUTviewport.TopLeftX = 0.0f;
	brdfLUTviewport.TopLeftY = 0.0f;
	//---
	device->CreateTexture2D(&brdfLUTDesc, 0, &brdfLUTtex);
	device->CreateRenderTargetView(brdfLUTtex, &brdfLUTRTVDesc, &brdfLUTRTV);
	device->CreateShaderResourceView(brdfLUTtex, &brdfLUTSRVDesc, &brdfLUTSRV);

	//deviceContext->RSSetState(rasterizerState.Get());
	//deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	//deviceContext->OMSetDepthStencilState(depthUnenableStencilState.Get(), 0);

	deviceContext->OMSetRenderTargets(1, &brdfLUTRTV, 0);
	deviceContext->RSSetViewports(1, &brdfLUTviewport);
	deviceContext->ClearRenderTargetView(brdfLUTRTV, color);

	auto vertexBuffer = quad.GetMesh().at(0).GetVertexBuffer();
	auto indexBuffer = quad.GetMesh().at(0).GetIndexBuffer();

	deviceContext->VSSetShader(vertexshader.GetShader(), 0, 0);
	deviceContext->PSSetShader(IntegrateBRDFPixelShader.GetShader(), 0, 0);


	this->cb_vs_vertexshader.data.wvpMatrix = DirectX::XMMatrixIdentity();// *  camViewMatrix * camProjMatrix;//m_GlobalInverseTransform
	this->cb_vs_vertexshader.data.worldMatrix = DirectX::XMMatrixIdentity(); 
	this->cb_vs_vertexshader.ApplyChanges();
	deviceContext->VSSetConstantBuffers(0, 1, this->cb_vs_vertexshader.GetAddressOf());

	stride = sizeof(Vertex3D);
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetInputLayout(vertexshader.GetInputLayout());
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(quad.GetMesh().at(0).indices.size(), 0, 0);

	/*QuadVertexShader->SetShader();
	IntegrateBRDFPixelShader->SetShader();


	ID3D11Buffer* nothing = 0;
	context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);

	context->Draw(3, 0);*/

	brdfLUTRTV->Release();

	//---JANKY CHEESE---
	//CreateWICTextureFromFile(device, context, L"Textures/ibl_brdf_lut.png", 0, &brdfLUTSRV);
	//---END CHEESE----

	
#pragma endregion
	//HRESULT hr = DirectX::CreateWICTextureFromFile(this->device.Get(), L"Data\\Textures\\brdfLUT.png", nullptr, brdfLUT.GetAddressOf());
	//COM_ERROR_IF_FAILED(hr, "Failed to create wic texture from file.");

	auto stop = std::chrono::high_resolution_clock::now();

	auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
	//printf("\n %f", (float)diff.count());
	std::cout << "\n" << diff.count();

	return true;
}

void Graphics::Fade(GameState gs)
{
	fade.fadestate = FadeState::Fade_Out;
	tempgs = gs;
}

void Graphics::SetDepthEnable(bool Enable)
{
	if (Enable)
	{
		deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	}
	else
	{
		deviceContext->OMSetDepthStencilState(depthUnenableStencilState.Get(), 0);
	}
}

void Graphics::LoadMap() {
	mapgo.clear();

	std::ifstream inFile;
	inFile.open("Map.txt");

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Map file\n";
		return;
	}
	else {
		std::cerr << "Map File found\n";
		std::string datafile = "";

		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{
				std::string filename;
				inFile >> filename;

				float px, py, pz;

				inFile >> px;
				inFile >> py;
				inFile >> pz;
			
				float rx, ry, rz;

				inFile >> rx;
				inFile >> ry;
				inFile >> rz;

				float sx, sy, sz;

				inFile >> sx;
				inFile >> sy;
				inFile >> sz;

				RenderableGameObject* go = new RenderableGameObject();
				go->Initialize(filename, this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader);
				go->SetPosition(XMFLOAT3(px,py,pz));
				go->SetRotation(XMFLOAT3(rx, ry, rz));
				go->SetScale(sx, sy, sz);
				go->SetGlobalMatirx(DirectX::XMMatrixIdentity());
				go->path = filename;
				mapgo.push_back(go);
			}
		}
	}
	mapgo.erase(mapgo.end() - 1);
}

ID3D11Device* Graphics::GetDevice() 
{
	return this->device.Get();
}

ID3D11DeviceContext* Graphics::GetDeviceContent()
{
	return this->deviceContext.Get();
}

void Graphics::EditorRayCast(XMFLOAT2 mousepos)
{
	XMVECTOR RayOrigin = this->Camera3D.GetPositionVector();
	auto front = this->Camera3D.GetForwardVector();
	XMVECTOR mousevec = XMVectorSet(mousepos.x, mousepos.y, 0, 0);

	auto matcam3d = //XMMatrixScalingFromVector(Camera3D.GetScaleVector()) * 
		XMMatrixRotationRollPitchYawFromVector(Camera3D.GetRotationVector())
		* XMMatrixTranslationFromVector(Camera3D.GetPositionVector());
	XMVECTOR RayDestination = XMVector3Unproject(mousevec, 0.0, 0.0,
		static_cast<float>(this->windowWidth), static_cast<float>(this->windowHeight), 0.0, 1.0,
		Camera3D.GetProjectionMatrix(), Camera3D.GetViewMatrix(), matcam3d);
	RayDestination = XMVector3Normalize(RayDestination);

	//min distance and index
	unsigned int minindx = 0;
	XMVECTOR mindis;

	std::vector<RenderableGameObject*> hitobject;
	for (size_t i = 0; i < mapgo.size(); i++)
	{
		float fDistance = -1.0f;
		float fDist;
		auto obb = mapgo.at(i)->GetCollisionObject()->obb;
		mapgo.at(i)->GetCollisionObject()->obb.Intersects(RayOrigin, RayDestination, fDist);
		
		fDistance = fDist;
		if (fDistance > 0)
		{
			hitobject.push_back(mapgo.at(i));
			
			XMVECTOR HitLocation = XMVectorMultiplyAdd(RayDestination, XMVectorReplicate(fDist), RayOrigin);
			auto dis = XMVector3Length(HitLocation - RayOrigin);
			if (i == 0)
			{
				mindis = dis;
			}
			else
			{
				auto res = XMVectorGreater(mindis, dis);
				auto b_res = XMVectorGetX(res);
				if (b_res == 1)
				{
					mindis = dis;
					minindx = hitobject.size() - 1;
				}
			}

			//temp
			m_editor.selectedGo = hitobject.at(minindx);
		}

	}
}

//bool Graphics::InitializeEffekseer()
//{
//	try
//	{
//		renderer = ::EffekseerRendererDX11::Renderer::Create(this->device.Get(), this->deviceContext.Get(), 8000);
//		// Create a manager of effects
//			// エフェクトのマネージャーの作成
//		manager = ::Effekseer::Manager::Create(8000);
//
//		// Sprcify rendering modules
//			// 描画モジュールの設定
//		manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
//		manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
//		manager->SetRingRenderer(renderer->CreateRingRenderer());
//		manager->SetTrackRenderer(renderer->CreateTrackRenderer());
//		manager->SetModelRenderer(renderer->CreateModelRenderer());
//
//		// Specify a texture, model, curve and material loader
//			// It can be extended by yourself. It is loaded from a file on now.
//			// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
//			// ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
//		manager->SetTextureLoader(renderer->CreateTextureLoader());
//		manager->SetModelLoader(renderer->CreateModelLoader());
//		manager->SetMaterialLoader(renderer->CreateMaterialLoader());
//		manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
//
//		// Specify a position of view
//			// 視点位置を確定
//		auto g_position = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);
//
//		// Specify a projection matrix
//			// 投影行列を設定
//		renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovLH(
//			90.0f / 180.0f * 3.14f, (float)this->windowWidth / (float)this->windowHeight, 1.0f, 3000.0f));
//
//		// Specify a camera matrix
//			// カメラ行列を設定
//		//renderer->SetCameraMatrix(
//		//	::Effekseer::Matrix44().LookAtLH(g_position, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));//
//		renderer->SetCameraMatrix(::Effekseer::Matrix44().Indentity());
//		// Load an effect
//			// エフェクトの読込
//		effect = Effekseer::Effect::Create(manager, (const EFK_CHAR*)L"Data/Eff/test.efk");//Data/Eff/tktk01/Sword1.efkproj
//
//	}
//	catch (COMException & exception)
//	{
//		ErrorLogger::Log(exception);
//		return false;
//	}
//	return true;
//}

//void Graphics::EffekseerUpdate() {
//	if (time % 120 == 0)
//	{
//		// Play an effect
//		// エフェクトの再生
//		handle = manager->Play(effect, 0, 0, 0);
//	}
//
//	if (time % 120 == 119)
//	{
//		// Stop effects
//		// エフェクトの停止
//		manager->StopEffect(handle);
//	}
//
//	// Move the effect
//	// エフェクトの移動
//	//manager->AddLocation(handle, ::Effekseer::Vector3D(0.2f, 0.0f, 0.0f));
//
//	// Update the manager
//	// マネージャーの更新
//	manager->Update();
//
//	// Update a time
//	// 時間を更新する
//	renderer->SetTime(time / 60.0f);
//}

//void Graphics::EffekseerDraw() 
//{
//	// Begin to rendering effects
//		// エフェクトの描画開始処理を行う。
//	renderer->BeginRendering();
//
//	// Render effects
//	// エフェクトの描画を行う。
//	manager->Draw();
//
//	// Finish to rendering effects
//	// エフェクトの描画終了処理を行う。
//	renderer->EndRendering();
//
//	time++;
//}
