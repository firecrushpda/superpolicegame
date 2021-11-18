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

	gfx.fade.UpdateFade();

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
					gfx.Fade(GameState::editor);
					//temp
					gfx.m_editor.InitializeEditor(gfx.GetDevice(),gfx.GetDeviceContent(), gfx.cb_vs_vertexshader);
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
					gfx.Fade(GameState::title);
				}
			}
		}
	}

	if (gfx.gs == GameState::game)
	{
		auto cameratype = gfx.Camera3D.cameratype;

		// キーボードの読み込む
		while (!keyboard.CharBufferIsEmpty())
		{
			unsigned char ch = keyboard.ReadChar();
		}

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
					
				}

				//camera
				if (keycode == 'L')//follow
				{
					gfx.Camera3D.cameratype = 0;
				}
				if (keycode == 'K')//free
				{
					gfx.Camera3D.cameratype = 1;
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

			//if (me.GetType() == MouseEvent::EventType::WheelDown)
			//{
			//
			//}


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
		if (cameratype == 0)
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
		}
		else if (cameratype == 1)
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
		if (mIsInput == false)
		{
			this->gfx.car.MoveFowards(dt, 0.0f);
		}

		//fix camera
		if (cameratype == 0)
		{
			auto viewrot = gfx.Camera3D.roundviewrot;
			auto carrot = gfx.car.carrender.GetRotationFloat3();
			if (viewrot.y != carrot.y)
			{
				auto viewrotV = XMLoadFloat3(&viewrot);
				auto carrotV = XMLoadFloat3(&carrot);
				auto lerpvalue = XMVectorLerp(viewrotV, carrotV, 0.035f);
				XMStoreFloat3(&gfx.Camera3D.roundviewrot, lerpvalue);
			}

			auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
			XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
			auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);

			auto testpos = gfx.car.carrender.GetPositionVector() + vec_backward * 10;
			DirectX::XMFLOAT3 temp;
			DirectX::XMStoreFloat3(&temp, testpos);
			temp = DirectX::XMFLOAT3(temp.x, temp.y + 10, temp.z);
			gfx.Camera3D.SetPosition(temp);
			gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
		}

		this->gfx.car.Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
		this->gfx.chasecar.Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
		gfx.cac->Update(1.0f/60.0f);

		auto vel = gfx.car.GetCarVelocity();
		auto maxspeed = gfx.car.GetMaxSpeed();
		gfx.car.carsui.Update(1.0f - std::abs(vel) / maxspeed, vel);

		mIsInput = false;

		//collision
		//衝突判定
		auto cocar = gfx.car.carrender.GetCollisionObject();
		auto cochasecar = gfx.chasecar.carrender.GetCollisionObject();
		auto cocamera = gfx.Camera3D.GetCameraCollision();

		DirectX::ContainmentType coresult = cocamera->frustum.Contains(cochasecar->obb);
		if (coresult == 2 || coresult == 1) {
			gfx.car.warninguiflag = true;
		}
		else
		{
			gfx.car.warninguiflag = false;
		}

		DirectX::ContainmentType coresult1 = cocar->obb.Contains(cochasecar->obb);
		if (coresult1 == 2 || coresult1 == 1) {
			//catch car
		}
	}

	if (gfx.gs == GameState::editor) 
	{
		// キーボードの読み込む
		while (!keyboard.CharBufferIsEmpty())
		{
			unsigned char ch = keyboard.ReadChar();
		}

		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == 'L')
				{ 
					gfx.m_editor.cameratype = 0;
					gfx.Camera3D.SetPosition(0, 10, -10);
					gfx.Camera3D.SetLookAtPos(XMFLOAT3(0, 0, 0));
				}

				if (keycode == 'K')
				{
					gfx.m_editor.cameratype = 1;
					gfx.Camera3D.SetPosition(0, 50, 0);
					gfx.Camera3D.SetLookAtPos(XMFLOAT3(0, 0, 0));
				}

				if (keycode == 'O')
				{
					gfx.showImgui = !gfx.showImgui;
				}

				if (keycode == 'I')
				{
					for (size_t i = 0; i < gfx.mapgo.size(); i++)
					{
						gfx.mapgo.at(i)->SetCollisionBoxView(true);
					}
				}

				if (keycode == 'P')
				{
					for (size_t i = 0; i < gfx.mapgo.size(); i++)
					{
						gfx.mapgo.at(i)->b_use = !gfx.mapgo.at(i)->b_use;
					}
				}

				if (keycode == '\r')
				{
					//gfx.Fade(GameState::title);
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

			if (me.GetType() == MouseEvent::EventType::MPress)
			{
				//raycast
				//gfx.EditorRayCast(mousepos);

				unsigned int minindx = -1;
				XMVECTOR mindis = XMVectorSet(999,999,999,999);
				std::vector<RenderableGameObject*> hitobject;
				std::vector<unsigned int> indexes;

				Ray ray = Ray::ScreenToRay(gfx.Camera3D, (float)me.GetPos().x, (float)me.GetPos().y);

				for (size_t i = 0; i < gfx.mapgo.size(); i++)
				{
					if (ray.Hit(gfx.mapgo.at(i)->GetCollisionObject()->obb))
					{
						hitobject.push_back(gfx.mapgo.at(i));
						indexes.push_back(i);
					}
				}
				
				for (size_t i = 0; i < hitobject.size(); i++)
				{
					auto dis = hitobject.at(i)->GetPositionVector() - gfx.Camera3D.GetPositionVector();
					dis = XMVector3Length(dis);
					if (XMVectorGetX(dis) <= XMVectorGetX(mindis))
					{
						mindis = dis;
						minindx = hitobject.size() - 1;
					}
				}
				if (minindx != -1)
				{
					gfx.m_editor.selectedGo = hitobject.at(minindx);
					gfx.m_editor.mapgoindex = indexes.at(minindx);
				}
					
			}

			if (mouse.IsRightDown() && gfx.m_editor.cameratype == 0)
			{

				if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
				{
					this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
				}
			}
		}

		float Camera3DSpeed = 0.06f;
		if (keyboard.KeyIsPressed(VK_SHIFT))
			Camera3DSpeed = 0.2f;


		if (gfx.m_editor.cameratype == 0)
		{
			if (keyboard.KeyIsPressed('W'))
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
			if (keyboard.KeyIsPressed('S'))
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
		}

		if (gfx.m_editor.cameratype == 1)
		{
			if (keyboard.KeyIsPressed('Q'))
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
			if (keyboard.KeyIsPressed('E'))
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
			if (keyboard.KeyIsPressed('W'))
			{
				auto rot = gfx.Camera3D.GetRotationFloat3();
				auto up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
				XMVECTOR vec_up = XMVector3TransformCoord(up, vecRotationMatrix);
				this->gfx.Camera3D.AdjustPosition(vec_up * Camera3DSpeed * dt);
			}
			if (keyboard.KeyIsPressed('S'))
			{
				auto rot = gfx.Camera3D.GetRotationFloat3();
				auto down = DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(rot.x, rot.y, 0.0f);
				XMVECTOR vec_down = XMVector3TransformCoord(down, vecRotationMatrix);
				this->gfx.Camera3D.AdjustPosition(vec_down * Camera3DSpeed * dt);
			}
		}
		
		if (keyboard.KeyIsPressed('A'))
			this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
		if (keyboard.KeyIsPressed('D'))
			this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
		if (keyboard.KeyIsPressed(VK_SPACE))
			this->gfx.Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);

		for (size_t i = 0; i < gfx.mapgo.size(); i++)
		{
			gfx.mapgo.at(i)->Update(1.0f,gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
		}
	}
	//fade
	auto fadestate = gfx.fade.fadestate;
	auto rate = gfx.fade.rate;
	//auto fadesprite = gfx.fade.fadesprite;
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
	
}
