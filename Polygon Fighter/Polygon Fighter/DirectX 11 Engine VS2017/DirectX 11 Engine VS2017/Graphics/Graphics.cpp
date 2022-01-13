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

	//sound
	m_Sound = new Sound();
	m_Sound->InitSound(hwnd);

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
		this->deviceContext->PSSetShaderResources(4, 1, disslovenoise.texture.get()->GetTextureResourceViewAddress());
		this->deviceContext->PSSetShaderResources(5, 1, &brdfLUTSRV);
		this->deviceContext->PSSetShaderResources(6, 1, &skyIBLSRV);
		this->deviceContext->PSSetShaderResources(7, 1, &envMapSRV);
		

		if (gs == GameState::title)
		{
			//stage.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			titlestage.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			//car.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix(), cb_ps_iblstatus);

			auto viewrot = Camera3D.roundviewrot;
			if (title.cam_updowmflag)
				Camera3D.roundviewrot = XMFLOAT3(viewrot.x, viewrot.y + title.cam_rotfrequance, viewrot.z);
			else
				Camera3D.roundviewrot = XMFLOAT3(viewrot.x, viewrot.y - title.cam_rotfrequance, viewrot.z);

			if (Camera3D.roundviewrot.y <= 2.9f || Camera3D.roundviewrot.y >= 3.5f)
				title.cam_updowmflag = !title.cam_updowmflag;

			auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
			XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(Camera3D.roundviewrot.x, Camera3D.roundviewrot.y, 0.0f);
			auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);

			auto testpos = car.carrender.GetPositionVector() + vec_backward * 250; //title cf_back
			DirectX::XMFLOAT3 temp;
			DirectX::XMStoreFloat3(&temp, testpos);
			temp = DirectX::XMFLOAT3(temp.x, temp.y + 50.0f, temp.z);//title cf_height
			Camera3D.SetPosition(temp);
			Camera3D.SetLookAtPos(car.carrender.GetPositionFloat3());
		}

		if (gs == GameState::game)
		{
			
			gameroad.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			zimen.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			//zimen1.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			//field_L.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			//field_R.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());

			//draw car
			car.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix(), cb_ps_iblstatus);

			cac->mAICar->Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix(), cb_ps_iblstatus);

			//draw npc
			//girl.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			for (size_t i = 0; i < npc.size(); i++) 
				npc.at(i)->Draw3D(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			

			//ステージ描画
			if (b_showmapgo) {
				for (size_t i = 0; i < mapgo.size(); i++)
					mapgo.at(i)->Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
			}
			if (b_showObstaclego)
			{
				for (size_t i = 0; i < obstaclego.size(); i++)
				{
					obstaclego.at(i)->Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());
				}
			}

			//draw temp phyx
			//for (int i = 0; i < tempmapgodm.size(); i++)
			//{
			//	//Update Constant buffer with WVP Matrix
			//	this->cb_vs_vertexshader.data.wvpMatrix = tempmapgodm[i].GetTransformMatrix() * Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix();//
			//	this->cb_vs_vertexshader.data.worldMatrix = tempmapgodm[i].GetTransformMatrix();//

			//	this->cb_vs_vertexshader.ApplyChanges();
			//	tempmapgodm[i].Draw();
			//}
			if (b_drawphysics)
				pgotest.Draw(Camera3D.GetViewMatrix() * Camera3D.GetProjectionMatrix());

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
		if (b_UIflag)
		{
			car.carsui.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix(),
				(car.warninguiflag && cac->possign_flag), !car.mCollsionOn, (cac->countdown > 0 && cac->hasbeenfound && cac->possign_flag));
			moneyui.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

			if (car.warninguiflag && cac->possign_flag)
				cac->possign.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

			for (size_t i = 0; i < npc.size(); i++)
				npc.at(i)->Draw2D(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

			if(b_thiflag)
			taihouimage.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

			if (b_siflag)
			shockimage.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		}
		
	}

	if (gs == GameState::tutorial)
	{
		tutorial_background.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		
		switch (tutorialtexno)
		{
		case 0:
			tutorial.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			break;
		case 1:
			tutorial2.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			break;
		}
	}

	if (gs == GameState::score)
	{
		score.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());

		if (scorepageindex == 0)
		{
			score01.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			catchcardig.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			rank1.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				scoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				tscoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		}
		else if (scorepageindex == 1)
		{
			rank.Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 3; i++)
				top1name[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				top1scoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 3; i++)
				top2name[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				top2scoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 3; i++)
				top3name[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				top3scoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 3; i++)
				top4name[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
			for (size_t i = 0; i < 6; i++)
				top4scoredigf[i].Draw(camera2D.GetWorldMatrix() * camera2D.GetOrthoMatrix());
		}
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
		auto co = car.actor->getGlobalPose().q;
		float angle; PxVec3 rot;
		co.toRadiansAndUnitAxis(angle, rot);
		PxMat44 trans = car.actor->getGlobalPose();
		 

		auto carpos = car.carrender.GetPositionFloat3();
		std::string pos = "pos" + std::to_string(carpos.x) + "_" + std::to_string(carpos.y) + "_" + std::to_string(carpos.z);//std::to_string(car.GetCarVelocity());//"rot" + std::to_string(co.getBasisVector0().x) + "_" + std::to_string(co.getBasisVector0().y) + "_" + std::to_string(co.getBasisVector0().z);
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
		std::wstring carposw = cv.from_bytes(pos);

		auto coccar = cac->mAICar->carrender.GetCollisionObject()->collisionuse;
		pos = "cochasecar " + std::to_string(coccar);
		std::wstring campos = cv.from_bytes(pos);

		auto velinstance = car.GetCarVelocity();
		auto vel = std::to_string(velinstance);//std::to_string(velinstance.x) + "_" + std::to_string(velinstance.y) + "_" + std::to_string(velinstance.z);
		std::wstring velutf8 = cv.from_bytes(vel);

		//collision text debug
		auto cocar = car.carrender.GetCollisionObject();
		auto cochasecar =  cac->mAICar->carrender.GetCollisionObject();
		auto cocamera = Camera3D.GetCameraCollision();
		auto triggernpc = npc.at(0)->starttrigger;

		DirectX::ContainmentType coresult1 = cocar->obb.Contains(npc.at(0)->starttrigger.obb);
		bool testbool = false;
		coresult1 == 0 ? testbool = true : testbool = false;

		std::string testbooltext = std::to_string(car.cardistance);
		std::wstring testboolutf8 = cv.from_bytes(testbooltext);

		if (b_debugUIflag)
		{
			spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
			//spriteFont->DrawString(spriteBatch.get(), carposw.c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			//spriteFont->DrawString(spriteBatch.get(), campos.c_str(), DirectX::XMFLOAT2(0, 20), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			//spriteFont->DrawString(spriteBatch.get(), velutf8.c_str(), DirectX::XMFLOAT2(0, 40), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			//spriteFont->DrawString(spriteBatch.get(), testboolutf8.c_str(), DirectX::XMFLOAT2(0, 60), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
			spriteBatch->End();
		}
	}
	if (gs == GameState::editor)
	{
		spriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
		spriteFont->DrawString(spriteBatch.get(), StringHelper::StringToWide(fpsString).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
		spriteBatch->End();
	}
	

	//サポートUI描画
	//Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (showImgui && gs == GameState::game)
	{
		//Create ImGui Test Window
		ImGui::Begin("Light Controls");
		ImGui::DragFloat("Metallic", &this->cb_ps_iblstatus.data.metallic, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("Roughness", &this->cb_ps_iblstatus.data.roughness, 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("dissolveThreshold", &this->cb_ps_iblstatus.data.dissolveThreshold, 0.01f, 0.0f, 1.0f);
		
		ImGui::NewLine();
		ImGui::DragFloat("FollowCameraFrontHeight", &Camera3D.cf_fheight, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("FollowCameraFront", &Camera3D.cf_front, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("FollowCameraHeight", &Camera3D.cf_height, 0.1f, 0.0f, 30.0f);
		ImGui::DragFloat("FollowCameraBack", &Camera3D.cf_back, 0.1f, 0.0f, 30.0f);
		ImGui::NewLine();
		ImGui::DragFloat3("mCarMaxSpeed", &car.mCarMaxSpeed.x, 0.1f, 0.0f, 8.0f);
		ImGui::DragFloat3("mCarAcceleration", &car.mCarAcceleration.x, 0.1f, 0.0f, 1.0f);
		ImGui::Checkbox("mCarAiContronlCanMove",&cac->canmove);
		ImGui::Checkbox("showMapGO", &b_showmapgo);
		ImGui::Checkbox("showPhysicsGO", &b_drawphysics);
		ImGui::NewLine();
		ImGui::Checkbox("UIflag", &b_UIflag);
		ImGui::Checkbox("b_debugUIflag", &b_debugUIflag);
		ImGui::DragFloat("Camera Works Wait Time(4)", &Camera3D.cwwaittime, 0.1f, 0.0f, 10.0f);
		ImGui::DragFloat("Camera Works Line Speed(5)", &Camera3D.cwlineSpeed, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Camera Works Rotate Distance(6)", &Camera3D.cwrotatedistance, 0.1f, 0.0f, 20.0f);
		ImGui::DragFloat("Camera Works Rotate Additional Height(6)", &Camera3D.cwrotateheight, 0.1f, 0.0f, 100.0f);
		ImGui::DragFloat("Camera Works Rotate Speed(6)", &Camera3D.cwrotatespeed, 0.01f, 0.0f, 1.0f);

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
			m_editor.selectedGo = nullptr;
			if (m_editor.mapgoindex != -1)
				mapgo.erase(mapgo.begin() + m_editor.mapgoindex);
		}

		if (ImGui::Button("Save"))
		{
			ofstream out("Map.txt");
			if (out.is_open())
			{
				for (size_t i = 0; i < mapgo.size(); i++)
				{
					//out << "This is a line.\n";
					//out << "This is another line.\n";
				}
				out.close();
			}
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
		scd.Windowed = TRUE; //TRUE FALSE
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
		tutorial.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight,"Data\\Textures\\tutorial_2.png" ,this->cb_vs_vertexshader_2d);
		tutorial2.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\tutorial1.png", this->cb_vs_vertexshader_2d);
		tutorial_background.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\fade_black.png", this->cb_vs_vertexshader_2d);

		//shock image for game pause
		taihouimage.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\cutin01.jpg", this->cb_vs_vertexshader_2d);
		shockimage.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\shock.png", this->cb_vs_vertexshader_2d);

		//score
		score.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\result.jpg", this->cb_vs_vertexshader_2d);
		score01.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\score01.png", this->cb_vs_vertexshader_2d);
		rank.Initialize(this->device.Get(), this->deviceContext.Get(), windowWidth, windowHeight, "Data\\Textures\\ranking01.png", this->cb_vs_vertexshader_2d);
		catchcardig.Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
		catchcardig.SetPosition(windowWidth / 2 - 200, windowHeight / 2 - 75, 0);
		rank1.Initialize(this->device.Get(), this->deviceContext.Get(), 49.7, 50, "Data/Textures/moji01.png", cb_vs_vertexshader_2d);
		rank1.SetPosition(windowWidth - 400, windowHeight - 150, 0);
		for (size_t i = 0; i < 6; i++)
		{
			scoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			scoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2 - 75, 0);
		}
		for (size_t i = 0; i < 6; i++)
		{
			tscoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			tscoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2, 0);
		}


		for (size_t i = 0; i < 3; i++)
		{
			top1name[i].Initialize(this->device.Get(), this->deviceContext.Get(), 49.7, 50, "Data/Textures/moji01.png", cb_vs_vertexshader_2d);
			top1name[i].SetPosition(windowWidth / 2  - 200 + i * 49.7, windowHeight / 2 - 125, 0);
		}
		for (size_t i = 0; i < 6; i++)
		{
			top1scoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			top1scoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2 - 120, 0);
		}
		for (size_t i = 0; i < 3; i++)
		{
			top2name[i].Initialize(this->device.Get(), this->deviceContext.Get(), 49.7, 50, "Data/Textures/moji01.png", cb_vs_vertexshader_2d);
			top2name[i].SetPosition(windowWidth / 2 - 200  + i * 49.7, windowHeight / 2 - 125 + 75, 0);
		}
		for (size_t i = 0; i < 6; i++)
		{
			top2scoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			top2scoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2 - 120 +75, 0);
		}
		for (size_t i = 0; i < 3; i++)
		{
			top3name[i].Initialize(this->device.Get(), this->deviceContext.Get(), 49.7, 50, "Data/Textures/moji01.png", cb_vs_vertexshader_2d);
			top3name[i].SetPosition(windowWidth / 2 - 200 + i * 49.7, windowHeight / 2 - 125 + 150, 0);
		}
		for (size_t i = 0; i < 6; i++)
		{
			top3scoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			top3scoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2 - 120 + 150, 0);
		}
		for (size_t i = 0; i < 3; i++)
		{
			top4name[i].Initialize(this->device.Get(), this->deviceContext.Get(), 49.7, 50, "Data/Textures/moji01.png", cb_vs_vertexshader_2d);
			top4name[i].SetPosition(windowWidth / 2 - 200 + i * 49.7, windowHeight / 2 - 125 + 225, 0);
		}
		for (size_t i = 0; i < 6; i++)
		{
			top4scoredigf[i].Initialize(this->device.Get(), this->deviceContext.Get(), 26.4, 40, "Data/Textures/score_m01.png", cb_vs_vertexshader_2d);
			top4scoredigf[i].SetPosition(windowWidth - 250 - 26.4 * (i + 1), windowHeight / 2  - 120 + 225, 0);
		}
		

		//stage
		if (!stage.Initialize("Data\\Objects\\stage.FBX", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;//Stage.FBX
		stage.SetCollisionBoxView(false);
		if (!titlestage.Initialize("Data\\Objects\\test\\title_2.fbx", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
		titlestage.SetCollisionBoxView(false);
		
		titlestage.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, XM_PI / 2, 0));
		titlestage.SetPosition(0, -100, 0);

		//game stage
		if (!gameroad.Initialize("Data\\Objects\\test\\douro_02.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
		gameroad.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, 0, 0));
		gameroad.SetCollisionBoxView(false);

		if (!zimen.Initialize("Data\\Objects\\test\\ground3.fbx", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
		zimen.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, 0, 0));
		zimen.SetCollisionBoxView(false);
		zimen.SetPosition(312.5, 0, - 11.78);
		zimen.SetScale(0.01, 0.01, 0.01);
		//zimen.SetScale(XMFLOAT3(10, 0, 10));
		//if (!zimen1.Initialize("Data\\Objects\\test\\zimen_02.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
		//	return false;
		//zimen1.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, 0, 0));
		//zimen1.SetCollisionBoxView(false);
		////zimen1.SetScale(XMFLOAT3(10, 0, 10));
		//if (!field_L.Initialize("Data\\Objects\\test\\field_L.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
		//	return false;
		//field_L.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, 0, 0));
		//field_L.SetCollisionBoxView(false);
		////field_L.SetScale(XMFLOAT3(100, 1, 1));
		////field_L.SetPosition(XMFLOAT3(0, 300, 0));
		//if (!field_R.Initialize("Data\\Objects\\test\\field_R.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
		//	return false;
		//field_R.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, 0, 0));
		//field_R.SetCollisionBoxView(false);
		//field_R.SetScale(XMFLOAT3(10, 0, 10));

		//game character car
		if(!car.CarInitialize("Data\\Objects\\test\\police.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader,true))
			return false;
		car.carrender.SetScale(1.5, 1.5, 1.5);
		car.taxirender.SetScale(0.65, 0.65, 0.65);
		//car.carrender.SetPosition(100, 0, 100);
		car.carbar.Initialize("Data\\Objects\\test\\p_steering.obj", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader);
		car.carbar.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(XM_PI, XM_PI / 2, 0));
		car.carbar.SetScale(0.1, 0.1, 0.1);
		car.carrender.SetCollisionBoxView(false);
		car.carbar.SetCollisionBoxView(false);
		
		car.m_Sound = this->m_Sound;

		//game chase car
		if (!chasecar.CarInitialize("Data\\Objects\\test\\enemy.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader,false))
			return false;
		chasecar.carrender.SetCollisionBoxView(false);
		chasecar.carbar.b_use = false;

		if (!chasecar1.CarInitialize("Data\\Objects\\test\\enemy02.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader, false))
			return false;
		chasecar1.carrender.SetCollisionBoxView(false);
		chasecar1.carrender.GetCollisionObject()->collisionuse = false;
		chasecar1.carrender.b_use = false;
		chasecar1.carbar.b_use = false;

		if (!chasecar2.CarInitialize("Data\\Objects\\test\\enemy03.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader, false))
			return false;
		chasecar2.carrender.SetCollisionBoxView(false);
		chasecar2.carrender.b_use = false;
		chasecar2.carrender.GetCollisionObject()->collisionuse = false;
		chasecar2.carbar.b_use = false;
		
		//car AI 
		cac = new CarAIController();
		//cac->cooldowmtimer.Start();
		cac->enemy = &chasecar;
		cac->enemy1 = &chasecar1;
		cac->enemy2 = &chasecar2;
		cac->SetAiCarByIndex();
		cac->ResetCarPosition();

		//load prerender quad
		if (!quad.Initialize("Data\\Objects\\quad.obj", this->device.Get(), this->deviceContext.Get(), this->cb_vs_vertexshader))
			return false;
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
		car.carsui.Initialize(this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader_2d,windowWidth,windowHeight);
		cac->possign.Initialize(this->device.Get(), this->deviceContext.Get(), 75, 77, "Data\\Textures\\marker01.png", cb_vs_vertexshader_2d);
		car.warningui.Initialize(this->device.Get(), this->deviceContext.Get(),96,37, "Data\\Textures\\warningtag.png", cb_vs_vertexshader_2d);
		car.warningui.SetPosition(XMFLOAT3(windowWidth / 2 - 48, windowHeight / 2 - 19, 0));
		moneyui.Initialize(this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader_2d, windowWidth, windowHeight,&car);

		//physics
		physxbase.initPhysics();
		car.actor = physxbase.gVehicle4W->getRigidDynamicActor();

		//load map game object
		LoadMap();

		//load obstacle
		LoadObstacle();

		//physics draw
		pgotest.Initialize("", this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader, &physxbase);

		//load npc object
		LoadNpc();

		//disslovenoise texture
		disslovenoise.Initialize(device.Get(), deviceContext.Get(), 300, 300,
			"Data\\Textures\\DissolveNoise.png", cb_vs_vertexshader_2d);

		//カメラ設置
		camera2D.SetProjectionValues(windowWidth, windowHeight, 0.0f, 1.0f);

		//game pause timer setting
		gamepausetimer.Start();

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
	cb_ps_iblstatus.data.dissolvelineWidth = 0.1;
	cb_ps_iblstatus.data.dissolveThreshold = 0.0f;
	cb_ps_iblstatus.data.dissolveColor = XMFLOAT4(1, 0, 0, 1);

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

				//compare primitive
				int primitiveindex = -1;
				if (filename != "")
					primitiveindex = CampareMapname(filename);

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

				if (filename!= "")
				{
					RenderableGameObject* go = new RenderableGameObject();
					if (primitiveindex == -1)
					{
						go->Initialize(filename, this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader);
						go->path = filename;
						mapgo.push_back(go);
						primitivesgo.push_back(go);
					}
					else
					{
						go->DeepCopy(*primitivesgo.at(primitiveindex));
						mapgo.push_back(go);
					}
					go->SetPosition(XMFLOAT3(px * 10 , py * 10 , pz * 10 ));
					go->SetRotation(XMFLOAT3(rx , ry, rz ));
					go->SetScale(sx * 10, sy * 10, sz  * 10);
					go->SetGlobalMatirx(DirectX::XMMatrixIdentity());
					go->SetCollisionBoxView(false);
				}
			}
		}
	}

	//read map gameobject and create buildings
	for (PxU32 i = 0; i < mapgo.size(); i++)
	{
		auto go = mapgo.at(i);
		auto co = go->GetCollisionObject();

		auto scale = go->GetScaleFloat3();
		auto rot = go->GetRotationFloat3();
		auto pos = go->GetPositionFloat3();
		auto trans = XMMatrixScaling(scale.x, scale.y, scale.z)
			* XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
			* XMMatrixTranslation(pos.x, pos.y, pos.z);

		auto xppos = XMVector3TransformCoord(XMVectorSet(co->originobb.Center.x, co->originobb.Center.y, co->originobb.Center.z, 0), trans);//
		XMFLOAT3 ttxppos; XMStoreFloat3(&ttxppos, xppos);
		auto quat = XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMFLOAT4 ttquat; XMStoreFloat4(&ttquat, quat);

		PxTransform t(PxVec3(ttxppos.x, ttxppos.y, ttxppos.z), PxQuat(ttquat.x, ttquat.y, ttquat.z, ttquat.w));
		PxRigidStatic* rd = physxbase.gPhysics->createRigidStatic(t);

		PxBoxGeometry boxGeom(PxVec3(co->originobb.Extents.x * scale.x,
								co->originobb.Extents.y * scale.y,
								co->originobb.Extents.z * scale.z));
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*rd, boxGeom, *physxbase.gMaterial);

		PxFilterData simFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_WHEEL, PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT, 0);
		shape->setSimulationFilterData(simFilterData);
		PxFilterData qryFilterData;
		setupDrivableSurface(qryFilterData);
		shape->setQueryFilterData(qryFilterData);

		physxbase.gScene->addActor(*rd);
	}
}

void Graphics::LoadObstacle() {
	obstaclego.clear();

	std::ifstream inFile;
	inFile.open("Obstacles.txt");

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Map file\n";
		return;
	}
	else {
		std::cerr << "Obstacle File found\n";
		std::string datafile = "";

		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{
				std::string filename;
				inFile >> filename;

				//compare primitive
				int primitiveindex = -1;
				if (filename != "")
					primitiveindex = CampareMapname(filename);

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

				if (filename != "")
				{
					RenderableGameObject* go = new RenderableGameObject();
					if (primitiveindex == -1)
					{
						go->Initialize(filename, this->device.Get(), this->deviceContext.Get(), cb_vs_vertexshader);
						go->path = filename;
						obstaclego.push_back(go);
						primitivesgo.push_back(go);
					}
					else
					{
						go->DeepCopy(*primitivesgo.at(primitiveindex));
						obstaclego.push_back(go);
					}
					go->SetPosition(XMFLOAT3(px, py, pz));
					go->SetRotation(XMFLOAT3(rx, ry, rz));
					go->SetScale(sx, sy, sz);
					go->SetGlobalMatirx(DirectX::XMMatrixIdentity());
					go->SetCollisionBoxView(false);
				}
			}
		}
	}

	//read obstacl gameobject and create buildings
	for (PxU32 i = 0; i < obstaclego.size(); i++)
	{
		auto go = obstaclego.at(i);
		auto co = go->GetCollisionObject();

		auto scale = go->GetScaleFloat3();
		auto rot = go->GetRotationFloat3();
		auto pos = go->GetPositionFloat3();
		auto trans = XMMatrixScaling(scale.x, scale.y, scale.z)
			* XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z)
			* XMMatrixTranslation(pos.x, pos.y, pos.z);

		auto xppos = XMVector3TransformCoord(XMVectorSet(co->originobb.Center.x, co->originobb.Center.y, co->originobb.Center.z, 0), trans);//
		XMFLOAT3 ttxppos; XMStoreFloat3(&ttxppos, xppos);
		auto quat = XMQuaternionRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMFLOAT4 ttquat; XMStoreFloat4(&ttquat, quat);

		PxTransform t(PxVec3(ttxppos.x, ttxppos.y + 1.7, ttxppos.z), PxQuat(ttquat.x, ttquat.y, ttquat.z, ttquat.w));
		PxRigidDynamic* rd = physxbase.gPhysics->createRigidDynamic(t);

		PxBoxGeometry boxGeom(PxVec3(co->originobb.Extents.x * scale.x,
									 co->originobb.Extents.y * scale.y,
									 co->originobb.Extents.z * scale.z));
		PxShape* shape = PxRigidActorExt::createExclusiveShape(*rd, boxGeom, *physxbase.gMaterial);

		PxFilterData simFilterData(COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_WHEEL, PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT, 0);
		shape->setSimulationFilterData(simFilterData);
		PxFilterData qryFilterData;
		setupDrivableSurface(qryFilterData);
		shape->setQueryFilterData(qryFilterData);

		obstaclephysics.push_back(rd);
		physxbase.gScene->addActor(*rd);
	}
}

//return true if primitive already has value
int Graphics::CampareMapname(std::string name)
{
	for (size_t i = 0; i < primitivesgo.size(); i++)
	{
		if (primitivesgo.at(i)->path == name)
		{
			return i;
		}
	}
	return -1;
}

void Graphics::LoadNpc()
{
	npc.clear();

	std::ifstream inFile;
	inFile.open("Npc.txt");

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Map file\n";
		return;
	}
	else {
		std::cerr << "Npc File found\n";

		std::string filename = "Data\\Objects\\test\\gile_wavehand.fbx";
		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{

				float px, py, pz;

				inFile >> px;
				inFile >> py;
				inFile >> pz;

				float pex, pey, pez;

				inFile >> pex;
				inFile >> pey;
				inFile >> pez;

				Npc* npcgo = new Npc();
				npcgo->Init(filename, this->device.Get(), this->deviceContext.Get(), 
						cb_vs_vertexshader, cb_vs_vertexshader_2d, &Camera3D,
						windowWidth,windowHeight);
				npcgo->girl.path = filename;
				npcgo->girl.SetPosition(XMFLOAT3(px, py, pz));
				npcgo->tgpos = XMFLOAT3(pex, pey, pez);
				npcgo->moneyui = &moneyui;
				npc.push_back(npcgo);
				npcgo->npcindex = npc.size() - 1;
				
			}
		}
	}
	npc.erase(npc.end() - 1);
}

ID3D11Device* Graphics::GetDevice() 
{
	return this->device.Get();
}

ID3D11DeviceContext* Graphics::GetDeviceContent()
{
	return this->deviceContext.Get();
}

void Graphics::ResetTitle()
{
	//reset title
	stage.b_use = true;
	titlestage.b_use = true;
	car.taxirender.b_modelview = false;
	car.carrender.SetScale(2, 2, 2);
	car.carrender.SetPosition(XMFLOAT3(0, 3, 0));
	car.cardrate = 0.0f;
	car.taxidrate = 1.0f;
	Camera3D.SetLookAtPos(car.carrender.GetPositionFloat3());

	auto viewrot = Camera3D.roundviewrot;
	auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(Camera3D.roundviewrot.x, Camera3D.roundviewrot.y, 0.0f);
	auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);

	auto testpos = car.carrender.GetPositionVector() + vec_backward * 10;
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, testpos);
	temp = DirectX::XMFLOAT3(temp.x, temp.y + Camera3D.cf_height, temp.z);
	Camera3D.SetPosition(temp);
	Camera3D.SetLookAtPos(car.carrender.GetPositionFloat3());

	Camera3D.roundviewrot = XMFLOAT3(viewrot.x, 3.0f, viewrot.z);

	m_Sound->StopSound();
	m_Sound->PlayIndexSound(Sound::SOUND_LABEL_BGM_BetterDays);
}

void Graphics::ResetGame()
{
	//reset game
	stage.b_use = false;
	titlestage.b_use = false;
	car.cardrate = 1.0f;
	car.taxidrate = 0.0f;
	car.carrender.SetScale(1.5, 1.5, 1.5);
	car.taxirender.b_modelview = true;
	Camera3D.roundviewrot = XMFLOAT3(0, 0, 0);
	PxTransform startTransform(PxVec3(924, 2.5f, -366.0f), PxQuat(PxIdentity));//-131 0 105
	physxbase.gVehicle4W->getRigidDynamicActor()->setGlobalPose(startTransform);
	physxbase.gVehicle4W->getRigidDynamicActor()->setLinearVelocity(PxVec3(0, 0, 0));

	//cac->currentcarindex = 0;
	//cac->SetAiCarByIndex();

	m_Sound->StopSound();
	m_Sound->PlayIndexSound(Sound::SOUND_LABEL_BGM_taxi);
}

void Graphics::ResetScore()
{
	for (size_t i = 0; i < npc.size(); i++)
	{
		auto tgnpc = npc.at(i);
		tgnpc->npcstate = 0;
		tgnpc->starttrigger.collisionuse = true;
		tgnpc->endtrigger.collisionuse = true;
	}
	car.haspassenger = false;
	moneyui.Hide();

	LoadScore();
	CompareScore();

	scorepageindex = 0;
	scorepagetimer = 0.0f;
}

void Graphics::LoadScore() {
	std::ifstream inFile;
	inFile.open("Score.txt");

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Map file\n";
		return;
	}
	else {
		std::cerr << "Npc File found\n";

		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{
				inFile >> top1[0];
				inFile >> top1[1];
				inFile >> top1[2];

				inFile >> top1score;

				inFile >> top2[0];
				inFile >> top2[1];
				inFile >> top2[2];

				inFile >> top2score;

				inFile >> top3[0];
				inFile >> top3[1];
				inFile >> top3[2];

				inFile >> top3score;

				inFile >> top4[0];
				inFile >> top4[1];
				inFile >> top4[2];

				inFile >> top4score;

			}
		}
	}

}

void Graphics::CompareScore() {
	float totalscore = gamescore * car.catchcount + 100;
	auto tscore = totalscore;
	tscore >= 999999 ? tscore = 999999 : tscore = tscore;
	if (tscore <= top4score)
	{
		editname = false;
	}
	else
	{
		if (tscore >= top1score) {
			for (size_t i = 0; i < 3; i++)
				top4[i] = top3[i];
			for (size_t i = 0; i < 3; i++)
				top3[i] = top2[i];
			for (size_t i = 0; i < 3; i++)
				top2[i] = top1[i];

			top4score = top3score;
			top3score = top2score;
			top2score = top1score;
			top1score = totalscore;
			editidx = 0;
		}
		else if(tscore < top1score && tscore >= top2score) {
			for (size_t i = 0; i < 3; i++)
				top3[i] = top2[i];
			for (size_t i = 0; i < 3; i++)
				top4[i] = top3[i];

			top4score = top3score;
			top3score = top2score;
			top2score = totalscore;
			editidx = 1;
		}
		else if (tscore < top2score && tscore >= top3score) {
			for (size_t i = 0; i < 3; i++)
				top4[i] = top3[i];

			top4score = top3score;
			top3score = totalscore;
			editidx = 2;
		}
		else if (tscore < top3score && tscore >= top4score) {

			top4score = totalscore;
			editidx = 3;
		}
	}
}


#pragma region not use
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


Mesh Graphics::TempProcessDebugMesh(const PxGeometry& geom)
{
	std::vector<Vertex3D> vertices;
	std::vector<DWORD> indices;

	switch (geom.getType())
	{
	case PxGeometryType::eBOX:
	{
		const PxBoxGeometry& boxGeom = static_cast<const PxBoxGeometry&>(geom);
		Mesh mesh = Mesh::DefaultGeometry(Mesh::PrimitiveGeomtry::box, device.Get(), deviceContext.Get(),
			"Date\\Textures\\fade_black.png", "PhysicGameobject");
		auto scale = XMMatrixScaling(boxGeom.halfExtents.x, boxGeom.halfExtents.y, boxGeom.halfExtents.z);
		mesh.transformMatrix = scale * mesh.transformMatrix;
		return mesh;
	}
	case PxGeometryType::eCONVEXMESH:
	{
		const PxConvexMeshGeometry& convexGeom = static_cast<const PxConvexMeshGeometry&>(geom);

		//Compute triangles for each polygon.
		const PxVec3& scale = convexGeom.scale.scale;
		PxConvexMesh* mesh = convexGeom.convexMesh;
		const PxU32 nbPolys = mesh->getNbPolygons();
		const PxU8* polygons = mesh->getIndexBuffer();
		const PxVec3* verts = mesh->getVertices();
		PxU32 nbVerts = mesh->getNbVertices();
		PX_UNUSED(nbVerts);

		PxU32 numTotalTriangles = 0;

		//バーテックス情報処理
		//Get vertices
		for (UINT i = 0; i < nbVerts; i++)
		{
			Vertex3D vertex;

			vertex.pos.x = verts[i].x;
			vertex.pos.y = verts[i].y;
			vertex.pos.z = verts[i].z;

			vertex.texCoord = XMFLOAT2(1, 1);

			vertex.bone_index.w = -1;
			vertex.bone_index.x = -1;
			vertex.bone_index.y = -1;
			vertex.bone_index.z = -1;

			vertex.bone_weights.w = -1;
			vertex.bone_weights.x = -1;
			vertex.bone_weights.y = -1;
			vertex.bone_weights.z = -1;

			vertices.push_back(vertex);
		}

		//インデックス情報
		for (PxU32 i = 0; i < nbPolys; i++)
		{
			PxHullPolygon data;
			mesh->getPolygonData(i, data);

			const PxU32 nbTris = PxU32(data.mNbVerts - 2);
			const PxU8 vref0 = polygons[data.mIndexBase + 0];
			PX_ASSERT(vref0 < nbVerts);
			for (PxU32 j = 0; j < nbTris; j++)
			{
				const PxU32 vref1 = polygons[data.mIndexBase + 0 + j + 1];
				const PxU32 vref2 = polygons[data.mIndexBase + 0 + j + 2];

				//generate face normal:
				PxVec3 e0 = verts[vref1] - verts[vref0];
				PxVec3 e1 = verts[vref2] - verts[vref0];

				PX_ASSERT(vref1 < nbVerts);
				PX_ASSERT(vref2 < nbVerts);

				PxVec3 fnormal = e0.cross(e1);
				fnormal.normalize();

				indices.push_back(vref0);
				indices.push_back(vref1);
				indices.push_back(vref2);

				vertices.at(vref0).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);
				vertices.at(vref1).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);
				vertices.at(vref2).normal = XMFLOAT3(fnormal.x, fnormal.y, fnormal.z);

			}
		}

		std::vector<Texture> textures;
		Texture diskTexture(device.Get(), "Date\\Textures\\fade_black.png", aiTextureType::aiTextureType_NONE);
		textures.push_back(diskTexture);

		return Mesh(device.Get(), deviceContext.Get(), vertices, indices, textures, DirectX::XMMatrixIdentity(), "Debugmesh");
	}
	break;
	}
}

#pragma endregion
