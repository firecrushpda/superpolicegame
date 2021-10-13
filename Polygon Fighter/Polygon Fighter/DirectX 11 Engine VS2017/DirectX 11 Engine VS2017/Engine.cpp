//=============================================================================
//
// �G���W������ [Engine.cpp]
// Author : GP11B183 28 �N �B�� (�p���@�C�^)
//
//=============================================================================
#include "Engine.h"

//=============================================================================
// �G���W������������
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
// ���b�Z�[�W����
//=============================================================================
bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

//=============================================================================
// �X�V����
//=============================================================================
void Engine::Update()
{
	// ���Ԃ̏�����
	float dt = timer.GetMilisecondsElapsed();
	timer.Restart();

	auto cameratype = gfx.Camera3D.cameratype;

	// �L�[�{�[�h�̓ǂݍ���
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
		}
	}

	 //�}�E�X�̓ǂݍ���
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

	//�Q�[���I�u�W�F�N�g�ɂ��Ă̏���

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
			this->gfx.car.carrender.AdjustRotation(XMFLOAT3(0, -0.001, 0));
		}
		if (keyboard.KeyIsPressed('D'))
		{
			this->gfx.car.carrender.AdjustRotation(XMFLOAT3(0, 0.001, 0));
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
	else if (cameratype == 1)
	{

	}

	if (keyboard.KeyIsPressed('C'))
	{
		XMVECTOR lightPosition = this->gfx.Camera3D.GetPositionVector();
		lightPosition += this->gfx.Camera3D.GetForwardVector();
		this->gfx.light.SetPosition(lightPosition);
		this->gfx.light.SetRotation(this->gfx.Camera3D.GetRotationFloat3());
	}

	auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetBackwardVector() * 10;
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, testpos);
	temp = DirectX::XMFLOAT3(temp.x, temp.y + 10, temp.z);
	gfx.Camera3D.SetPosition(temp);
	gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());

	//collision
	//�Փ˔���
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
// �`�揈��
//=============================================================================
void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

