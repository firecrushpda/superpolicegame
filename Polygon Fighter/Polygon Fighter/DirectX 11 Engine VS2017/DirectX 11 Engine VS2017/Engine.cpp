//=============================================================================
//
// エンジン処理 [Engine.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Engine.h"

//=============================================================================
// エンジン初期化処理
//=============================================================================
bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	timer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	gfx.ResetTitle();
	return true;
}

//=============================================================================
// メッセージ処理
//=============================================================================
bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

//=============================================================================
// 更新処理
//=============================================================================
void Engine::Update()
{
	// 時間の初期化
	float dt = 1.0;//timer.GetMilisecondsElapsed();
	timer.Restart();

	if (gfx.gs == GameState::title)
	{
		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == '\r')
				{
					gfx.Fade(GameState::game);
					//gfx.Fade(GameState::editor);
					////temp
					//gfx.m_editor.InitializeEditor(gfx.GetDevice(),gfx.GetDeviceContent(), gfx.cb_vs_vertexshader);
				}
				if (keycode == 'T')
				{
					gfx.Fade(GameState::tutorial);
				}
			}
		}
	}

	if (gfx.gs == GameState::tutorial)
	{
		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == '\r')
				{
					gfx.tutorialtexno++;
					if (gfx.tutorialtexno >= 2)
						gfx.Fade(GameState::title);
				}
			}
		}
	}

	if (gfx.gs == GameState::score)
	{
		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == '\r')
				{
					gfx.Fade(GameState::title);
				}
			}
		}
	}
#pragma region GameState::game
	if (gfx.gs == GameState::game)
	{
		if (!gfx.gamepause)
		{
			auto cameratype = gfx.Camera3D.cameratype;

			// キーボードの読み込む
			while (!keyboard.CharBufferIsEmpty())
			{
				unsigned char ch = keyboard.ReadChar();
			}

			//key press
			while (!keyboard.KeyBufferIsEmpty())
			{
				KeyboardEvent kbe = keyboard.ReadKey();
				unsigned char keycode = kbe.GetKeyCode();
				if (kbe.IsPress())
				{
					if (keycode == 'O')
					{
						gfx.showImgui = !gfx.showImgui;
					}
					if (keycode == 'P')
					{
						gfx.car.carrender.SetCollisionBoxView(true);
						for (size_t i = 0; i < gfx.mapgo.size(); i++)
						{
							gfx.mapgo.at(i)->SetCollisionBoxView(true);
						}
					}

					//camera
					if (keycode == '0')//follow
					{
						gfx.Camera3D.cameratype = 0;
						gfx.car.carbar_drawflag = false;
					}
					if (keycode == '1')//free
					{
						gfx.Camera3D.cameratype = 1;
						gfx.car.carbar_drawflag = false;
					}
					if (keycode == '2')//front
					{
						gfx.Camera3D.cameratype = 2;
						gfx.car.carbar_drawflag = true;
					}


					if (keycode == '\r')
					{
						gfx.Fade(GameState::score);
					}
				}

				if (kbe.IsRelease())
				{

				}

			}

			//マウスの読み込む
			while (!mouse.EventBufferIsEmpty())
			{
				MouseEvent me = mouse.ReadEvent();

				if (mouse.IsRightDown())
				{
					if (cameratype == 1)
					{
						if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
						{
							this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
						}
					}
				}
			}

			float Camera3DSpeed = 0.06f;

			//press
			if (cameratype == 1)
			{
				if (keyboard.KeyIsPressed('W'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('S'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('A'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('D'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed(VK_SPACE))
				{
					this->gfx.Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
				}
				if (keyboard.KeyIsPressed('Z'))
				{
					this->gfx.Camera3D.AdjustPosition(0.0f, -Camera3DSpeed * dt, 0.0f);
				}
			}

			//fix camera
			if (cameratype == 0)
			{
				/*if (speedupflag)
				{
					this->gfx.car.MoveFowards(dt, 1.0f);
					mIsInput = true;
				}

				if (speeddownflag)
				{
					this->gfx.car.MoveFowards(dt, -1.0f);
					mIsInput = true;
				}*/

				if (keyboard.KeyIsPressed('W'))
				{
					this->gfx.car.MoveFowards(dt, 1.0f);
					mIsInput = true;
				}
				else if (keyboard.KeyIsPressed('S'))
				{
					this->gfx.car.MoveFowards(dt, -1.0f);
					mIsInput = true;
				}

				if (keyboard.KeyIsPressed('A'))
				{
					this->gfx.car.Turn(dt, -1.0f);
				}
				else if (keyboard.KeyIsPressed('D'))
				{
					this->gfx.car.Turn(dt, 1.0f);
				}
				else
				{
					this->gfx.car.Turn(dt, 0.0f);
				}
			}

			if (cameratype == 2)
			{

				if (keyboard.KeyIsPressed('W'))
				{
					this->gfx.car.MoveFowards(dt, 1.0f);
					mIsInput = true;
				}
				else if (keyboard.KeyIsPressed('S'))
				{
					this->gfx.car.MoveFowards(dt, -1.0f);
					mIsInput = true;
				}

				if (keyboard.KeyIsPressed('A'))
				{
					this->gfx.car.Turn(dt, -1.0f);
				}
				else if (keyboard.KeyIsPressed('D'))
				{
					this->gfx.car.Turn(dt, 1.0f);
				}
				else
				{
					this->gfx.car.Turn(dt, 0.0f);
				}

				auto viewrot = gfx.Camera3D.roundviewrot;
				auto carrot = gfx.car.carrender.GetRotationFloat3();
				if (viewrot.y != carrot.y)
				{
					auto viewrotV = XMLoadFloat3(&viewrot);
					auto carrotV = XMLoadFloat3(&carrot);
					auto lerpvalue = XMVectorLerp(viewrotV, carrotV, 0.035f);
					XMStoreFloat3(&gfx.Camera3D.roundviewrot, lerpvalue);
				}

				auto dfront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
				auto vec_front = XMVector3TransformCoord(dfront, vecRotationMatrix);

				auto testpos = gfx.car.carrender.GetPositionVector() + vec_front * gfx.Camera3D.cf_front;
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, testpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y + gfx.Camera3D.cf_fheight, temp.z);
				gfx.Camera3D.SetPosition(temp);

				//car bar pos
				auto barpos = gfx.car.carrender.GetPositionVector() + vec_front * (gfx.Camera3D.cf_front + 0.3);
				DirectX::XMStoreFloat3(&temp, barpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y - 0.2, temp.z);

				//rot
				auto lacarbarrotZ = gfx.car.carbar.GetRotationFloat3().z;
				lacarbarrotZ = lacarbarrotZ * (1 - 0.05);
				gfx.car.carbar.SetPosition(temp);
				gfx.car.carbar.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				auto lacarbarrot = gfx.car.carbar.GetRotationFloat3();

				gfx.car.carbar.SetRotation(XMFLOAT3(lacarbarrot.x, lacarbarrot.y + XM_PI, lacarbarrotZ));//-gfx.Camera3D.roundviewrot.y

				testpos = gfx.car.carrender.GetPositionVector() + vec_front * gfx.Camera3D.cf_front * 10;
				XMStoreFloat3(&temp, testpos);
				gfx.Camera3D.SetLookAtPos(temp);
			}

			if (mIsInput == false)
			{
				this->gfx.car.MoveFowards(dt, 0.0f);
			}
			//update car
			this->gfx.car.Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());

			//update car speed ui
			auto vel = gfx.car.GetCarVelocity();
			auto maxspeed = gfx.car.GetMaxSpeed();
			gfx.car.carsui.Update(1.0f - std::abs(vel) / maxspeed, vel);

			//update money ui
			gfx.moneyui.Update();

			//update chase car
			this->gfx.chasecar.Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
			gfx.cac->Update(1.0f / 60.0f);

			//update chase car position sign
			auto chasecarvecpos = gfx.chasecar.carrender.GetPositionVector();
			auto camviewport = gfx.Camera3D.viewport;
			auto vec = XMVector3Project(chasecarvecpos, camviewport.TopLeftX, camviewport.TopLeftY,
				camviewport.Width, camviewport.Height, camviewport.MinDepth, camviewport.MaxDepth,
				gfx.Camera3D.GetProjectionMatrix(), gfx.Camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());
			XMFLOAT3 co;
			XMStoreFloat3(&co, vec);
			co = XMFLOAT3(clamp(co.x, -200.0f, gfx.windowWidth + 100.0f), clamp(co.y, -200.0f, gfx.windowHeight + 100.0f) - 25, 0);
			gfx.cac->possign.SetPosition(co);

			if (cameratype == 0)
			{
				//camrot
				auto viewrot = gfx.Camera3D.roundviewrot;
				auto carrot = gfx.car.carrender.GetRotationFloat3();
				if (viewrot.y != carrot.y)
				{
					auto viewrotV = XMLoadFloat3(&viewrot);
					auto carrotV = XMLoadFloat3(&carrot);
					auto lerpvalue = XMVectorLerp(viewrotV, carrotV, 0.035f);
					XMStoreFloat3(&gfx.Camera3D.roundviewrot, lerpvalue);
				}

				//campos
				auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
				auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);
				auto testpos = gfx.car.carrender.GetPositionVector() + vec_backward * gfx.Camera3D.cf_back;
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, testpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y + gfx.Camera3D.cf_height, temp.z);
				gfx.Camera3D.SetPosition(temp);
				gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
			}


			if (cameratype == 3)
			{
				auto npc = gfx.npc.at(gfx.currentnpcindex);
				auto dback = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(npc->girl.GetRotationFloat3().x, npc->girl.GetRotationFloat3().y, 0.0f);
				auto vec_forward = XMVector3TransformCoord(dback, vecRotationMatrix);
				auto testpos = npc->girl.GetPositionVector() + vec_forward * 10 + DirectX::XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);//
				gfx.Camera3D.SetPosition(testpos);
				//gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());

				//camrot
				auto viewrot = gfx.Camera3D.roundviewrot;
				auto lookAtPos = npc->girl.GetPositionFloat3();
				auto pos = gfx.Camera3D.GetPositionFloat3();

				gfx.Camera3D.SetLookAtPos(lookAtPos);

			}

			//input sign
			mIsInput = false;

			//update map game object
			for (size_t i = 0; i < gfx.mapgo.size(); i++)
				gfx.mapgo.at(i)->Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());

			//update npc
			for (size_t i = 0; i < gfx.npc.size(); i++)
				gfx.npc.at(i)->Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());

			//collision
			//衝突判定
			auto cocar = gfx.car.carrender.GetCollisionObject();
			auto cochasecar = gfx.chasecar.carrender.GetCollisionObject();
			auto cocamera = gfx.Camera3D.GetCameraCollision();

			for (size_t i = 0; i < gfx.mapgo.size(); i++)
			{
				auto obb = gfx.mapgo.at(i)->GetCollisionObject()->obb;
				DirectX::ContainmentType coresult = cocamera->frustum.Contains(obb);
				if (coresult == 2 || coresult == 1)
					gfx.mapgo.at(i)->b_modelview = true;
				else
					gfx.mapgo.at(i)->b_modelview = false;
			}

			

			if (cochasecar->collisionuse)
			{
				DirectX::ContainmentType coresult = cocamera->frustum.Contains(cochasecar->obb);
				if (coresult == 2 || coresult == 1)
					gfx.car.warninguiflag = true;
				else
					gfx.car.warninguiflag = false;

				coresult = cocar->obb.Contains(cochasecar->obb);
				if (coresult == 2 || coresult == 1) {
					//catch car
					gfx.gamepause = true;

					//reset catch car animaiton status
					gfx.catchcar_animationsequenceflag = true;
					gfx.cac->possign_flag = false;
					gfx.catchcar_animaitonindex = 0;
					gfx.gamepausetimer.Restart();
					gfx.Camera3D.DeepCopyTo(&tempcam);
				}
			}
			

			//npc collision
			for (size_t i = 0; i < gfx.npc.size(); i++)
			{
				auto npc = gfx.npc.at(i);
				if (npc->npcstate <= 1)
				{
					DirectX::ContainmentType coresult = cocar->obb.Contains(npc->starttrigger.obb);
					if (npc->npcstate == 0 && (coresult == 2 || coresult == 1)
						&& std::fabs(vel) <= 0.5 && !gfx.car.haspassenger && npc->starttrigger.collisionuse)
					{
						//change npcstate
						npc->starttrigger.collisionuse = false;
						gfx.Camera3D.cameratype = 3;
						npc->npcstate = 1;
						npc->npctimer.Restart();
						npc->uiflag = true;
						gfx.car.haspassenger = true;
						gfx.currentnpcindex = npc->npcindex;
					}
				}
				if (npc->npcstate >= 2)
				{
					if (npc->endtrigger.collisionuse)
					{
						DirectX::ContainmentType coresult = cocar->obb.Contains(npc->endtrigger.obb);
						if ((coresult == 2 || coresult == 1) && std::fabs(vel) <= 0.5 && gfx.car.haspassenger)
						{
							//change npcstate
							npc->npcstate = 3;
							gfx.moneyui.Hide();
							gfx.car.haspassenger = false;
							npc->endtrigger.collisionuse = false;

							//
							gfx.gamescore += gfx.car.cardistance / 3.0f * 57;
						}
					}
				}
			}
		}
		else
		{
			//catch car animation
			//change camera to 3 position and stay 1/3 second
			//make some noise
			if (gfx.catchcar_animationsequenceflag) 
			{
				auto pausetime = gfx.gamepausetimer.GetMilisecondsElapsed();
				if (pausetime >= 1000){
					//voice
					//change camera
					gfx.catchcar_animaitonindex++;
					gfx.gamepausetimer.Restart();
					if (gfx.catchcar_animaitonindex >= gfx.catchcar_animaitonmax)
					{
						//fadeback to the screen
						gfx.gamepause = false;
						gfx.catchcar_animationsequenceflag = false;
						gfx.catchcar_animaitonindex = 0;
						//caught setting
						gfx.cac->possign_flag = false;
						gfx.chasecar.carrender.b_use = false;
						gfx.chasecar.carrender.GetCollisionObject()->collisionuse = false;
						gfx.gamepausetimer.Stop();
						gfx.Fade(GameState::game);
						//tempcam.DeepCopyTo(&gfx.Camera3D);
					}
				}

				if (gfx.catchcar_animaitonindex == 0)
				{
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetForwardVector() * 2;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y + 3, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
				else if (gfx.catchcar_animaitonindex == 1)
				{
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetLeftVector() * 2;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y - 0.3, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
				else if (gfx.catchcar_animaitonindex == 2)
				{
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetRightVector() * 2;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y + 1, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
			}
		}
	}
#pragma endregion
#pragma region GameState::editor
	//if (gfx.gs == GameState::editor) 
//{
//	// キーボードの読み込む
//	while (!keyboard.CharBufferIsEmpty())
//	{
//		unsigned char ch = keyboard.ReadChar();
//	}

//	while (!keyboard.KeyBufferIsEmpty())
//	{
//		KeyboardEvent kbe = keyboard.ReadKey();
//		unsigned char keycode = kbe.GetKeyCode();
//		if (kbe.IsPress())
//		{
//			if (keycode == 'O')
//			{
//				gfx.showImgui = !gfx.showImgui;
//			}

//			if (keycode == 'I')
//			{
//				for (size_t i = 0; i < gfx.mapgo.size(); i++)
//				{
//					gfx.mapgo.at(i)->SetCollisionBoxView(true);
//				}
//			}

//			if (keycode == 'P')
//			{
//				for (size_t i = 0; i < gfx.mapgo.size(); i++)
//				{
//					gfx.mapgo.at(i)->b_use = !gfx.mapgo.at(i)->b_use;
//				}
//			}

//			if (keycode == '\r')
//			{
//				//gfx.Fade(GameState::title);
//			}
//		}

//		if (kbe.IsRelease())
//		{

//		}

//	}

//	//マウスの読み込む
//	while (!mouse.EventBufferIsEmpty())
//	{
//		MouseEvent me = mouse.ReadEvent();

//		if (me.GetType() == MouseEvent::EventType::MPress)
//		{
//			//raycast
//			//gfx.EditorRayCast(mousepos);

//			unsigned int minindx = -1;
//			XMVECTOR mindis = XMVectorSet(999,999,999,999);
//			std::vector<RenderableGameObject*> hitobject;
//			std::vector<unsigned int> indexes;

//			Ray ray = Ray::ScreenToRay(gfx.Camera3D, (float)me.GetPos().x, (float)me.GetPos().y);

//			for (size_t i = 0; i < gfx.mapgo.size(); i++)
//			{
//				if (ray.Hit(gfx.mapgo.at(i)->GetCollisionObject()->obb))
//				{
//					hitobject.push_back(gfx.mapgo.at(i));
//					indexes.push_back(i);
//				}
//			}
//			
//			for (size_t i = 0; i < hitobject.size(); i++)
//			{
//				auto dis = hitobject.at(i)->GetPositionVector() - gfx.Camera3D.GetPositionVector();
//				dis = XMVector3Length(dis);
//				if (XMVectorGetX(dis) <= XMVectorGetX(mindis))
//				{
//					mindis = dis;
//					minindx = hitobject.size() - 1;
//				}
//			}
//			if (minindx != -1)
//			{
//				gfx.m_editor.selectedGo = hitobject.at(minindx);
//				gfx.m_editor.mapgoindex = indexes.at(minindx);
//			}
//				
//		}

//		if (mouse.IsRightDown())
//		{

//			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
//			{
//				this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
//			}
//		}
//	}

//	float Camera3DSpeed = 0.06f;

//	if (keyboard.KeyIsPressed('W'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('S'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('A'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('D'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed(VK_SPACE))
//	{
//		this->gfx.Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
//	}

//	for (size_t i = 0; i < gfx.mapgo.size(); i++)
//	{
//		gfx.mapgo.at(i)->Update(1.0f,gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
//	}
//}
#pragma endregion

	//fade
	auto fadestate = gfx.fade.fadestate;
	auto rate = gfx.fade.rate;
	if (fadestate == FadeState::Fade_In)
	{
		auto alpha = gfx.fade.fadesprite.color.w - rate;
		alpha = std::clamp(alpha, 0.0f, 1.0f);
		gfx.fade.fadesprite.color = XMFLOAT4(gfx.fade.fadesprite.color.x,
			gfx.fade.fadesprite.color.y, gfx.fade.fadesprite.color.z, alpha);
		alpha <= 0.0f ? fadestate = FadeState::Fade_none : FadeState::Fade_In;
	}
	if (fadestate == FadeState::Fade_Out)
	{
		auto alpha = gfx.fade.fadesprite.color.w + rate;
		alpha = std::clamp(alpha, 0.0f, 1.0f);
		gfx.fade.fadesprite.color = XMFLOAT4(gfx.fade.fadesprite.color.x,
			gfx.fade.fadesprite.color.y, gfx.fade.fadesprite.color.z, alpha);
		if (alpha >= 1.0f)
		{
			gfx.fade.fadestate = FadeState::Fade_In;

			//change game state
			gfx.gs = gfx.tempgs;
			ChangeStats(gfx.gs);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

//=============================================================================
// ゲーム状態チェンジする
//=============================================================================
void Engine::ChangeStats(GameState state) 
{
	switch (state)
	{
	case GameState::title:
		//reset title
		gfx.ResetTitle();
		break;
	case GameState::game:
		gfx.ResetGame();
		break;
	case GameState::score:
		gfx.stage.b_use = false;
		break;
	case GameState::tutorial:
		//reset tutorial
		gfx.tutorialtexno = 0;
		gfx.stage.b_use = false;
		break;
	case GameState::catchcar:
		//catch car fade
		gfx.stage.b_use = false;
		break;
	}
}
