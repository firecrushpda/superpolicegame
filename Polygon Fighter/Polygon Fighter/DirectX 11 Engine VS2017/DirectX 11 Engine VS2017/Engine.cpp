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
				gfx.car.carrender.SetSkeletonDebugFlag();
			}

			//camera
			if (keycode == '0')//follow
			{
				gfx.Camera3D.cameratype = 0;
			}
			if (keycode == '1')//free
			{
				gfx.Camera3D.cameratype = 1;
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

		if (me.GetType() == MouseEvent::EventType::WheelDown)
		{
			
		}


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

	gfx.cac->Update(1.0f/60.0f);

	auto vel = gfx.car.GetCarVelocity();
	auto maxspeed = gfx.car.GetMaxSpeed();
	gfx.car.carsui.Update(1.0f - std::abs(vel) / maxspeed, vel);

	mIsInput = false;

	//collision
	//衝突判定
	//auto playerblade = gfx.gameObject.GetBladeCollsionObject();
	//auto playerbody = gfx.gameObject.GetBodyCollsionObject();
	//auto enemyblade = gfx.enemy.GetBladeCollsionObject();
	//auto enemybody = gfx.enemy.GetBodyCollsionObject();

	//DirectX::ContainmentType coresult = playerblade->aabb.Contains(enemybody->aabb);
	//if (precoreslut == 0 && coresult != 0)
	//{
	//	//hit

	//	gfx.enemy.hp -= 30;
	//	if (gfx.enemy.hp >= 0)
	//	{
	//		gfx.enemy.PlayAnimation(1, AnimationPlayStyle::PlayOnce);
	//	}
	//	else
	//	{
	//		auto index = gfx.enemy.GetCurrentAnimationClipIndex();
	//		if (index != 7)
	//		{
	//			gfx.enemy.PlayAnimation(7, AnimationPlayStyle::PlayOnce);
	//		}
	//	}
	//}
	//precoreslut = coresult;

	
}

//=============================================================================
// 描画処理
//=============================================================================
void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

