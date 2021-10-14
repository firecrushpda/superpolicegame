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
	float dt = timer.GetMilisecondsElapsed();
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
			if (keycode == VK_SHIFT)
			{
				gfx.car.accflag = true;
			}
		}

		if (kbe.IsRelease())
		{
			gfx.car.accflag = false;
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
			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
			{
				this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
			}
		}
	}

	float Camera3DSpeed = 0.06f;

	if (cameratype == 0)
	{
		if (keyboard.KeyIsPressed('W'))
		{
			this->gfx.car.carrender.AdjustPosition(this->gfx.car.carrender.GetForwardVector() * Camera3DSpeed * dt);
		}
		if (keyboard.KeyIsPressed('S'))
		{
			this->gfx.car.carrender.AdjustPosition(this->gfx.car.carrender.GetBackwardVector() * Camera3DSpeed * dt);
		}
		if (keyboard.KeyIsPressed('A'))
		{
			this->gfx.car.carrender.AdjustRotation(XMFLOAT3(0, -gfx.car.rotatespeed, 0));
		}
		if (keyboard.KeyIsPressed('D'))
		{
			this->gfx.car.carrender.AdjustRotation(XMFLOAT3(0, gfx.car.rotatespeed, 0));
		}
		if (keyboard.KeyIsPressed(VK_SHIFT))
		{
			if (keyboard.KeyIsPressed('W'))
			{
				gfx.car.verticalacc += gfx.car.accincrease;
			}
			if (keyboard.KeyIsPressed('S'))
			{
				gfx.car.verticalacc -= gfx.car.accincrease;
			}
			if (keyboard.KeyIsPressed('A'))
			{
				gfx.car.hoizontalacc += gfx.car.accincrease;
			}
			if (keyboard.KeyIsPressed('D'))
			{
				gfx.car.hoizontalacc -= gfx.car.accincrease;
			}
		}
	}
	else if (cameratype == 1)
	{

	}

	//acc adjust
	if (!gfx.car.accflag)
	{
		gfx.car.hoizontalacc--;
		gfx.car.verticalacc--;
		gfx.car.acc--;
	}


	//fix camera
	auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetBackwardVector() * 10;
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, testpos);
	temp = DirectX::XMFLOAT3(temp.x, temp.y + 10, temp.z);
	gfx.Camera3D.SetPosition(temp);
	gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());

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

