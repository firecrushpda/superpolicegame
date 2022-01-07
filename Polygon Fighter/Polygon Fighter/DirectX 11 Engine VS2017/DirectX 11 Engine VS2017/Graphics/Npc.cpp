#include "Npc.h"

bool Npc::Init(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext,
	ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, Camera3D* camera3d,float windowwidth,float windowheight)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->camera3d = camera3d;
	this->windowwidth = windowwidth;
	this->windowheight = windowheight;

	npctimer.Start();

	if (!girl.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;
	girl.SetPosition(0, 1, 0);
	girl.SetScale(0.005, 0.005, 0.005);
	girl.SetRotation(XMFLOAT3(-XM_PI / 2, 0, 0));
	girl.SetCollisionBoxView(false);

	//sprite
	npcUI_thank.Initialize(device, deviceContext, 400, 200, "Data\\Textures\\npcUIthank.png", cb_vs_vertexshader_2d);
	npcUI_thank.SetPosition(XMFLOAT3(windowwidth / 2 - 200, windowwidth - 100, 0));
	npcUI_reach.Initialize(device, deviceContext, 576, 32, "Data\\Textures\\ride01.png", cb_vs_vertexshader_2d);
	npcUI_reach.SetPosition(XMFLOAT3(windowwidth / 2 - 288, 35, 0));

	//pos sign
	possign.Initialize(device, deviceContext, 75, 77, "Data\\Textures\\mokuteki.png", cb_vs_vertexshader_2d);
	desdirsign.Initialize(device, deviceContext, 76, 132, "Data\\Textures\\yajirushi01.png", cb_vs_vertexshader_2d);
	desdirsign.SetPosition(XMFLOAT3(windowwidth/2 - 76,windowheight/2 - 132,0));

	//trigger
	starttrigger = *girl.GetCollisionObject();
	starttrigger.originobb.Extents = XMFLOAT3(starttrigger.originobb.Extents.x * npctrigger_distance,
		starttrigger.originobb.Extents.y * npctrigger_distance,
		starttrigger.originobb.Extents.z * npctrigger_distance);
	starttrigger.collisionuse = true;

	endtrigger = *girl.GetCollisionObject();
	endtrigger.originobb.Extents = XMFLOAT3(endtrigger.originobb.Extents.x * npctrigger_distance,
		endtrigger.originobb.Extents.y * npctrigger_distance,
		endtrigger.originobb.Extents.z * npctrigger_distance);
	endtrigger.collisionuse = true;

	return true;
}

void Npc::Update(float dt, const XMMATRIX & viewProjectionMatrix,XMFLOAT3 carpos)
{
	girl.Update(dt, viewProjectionMatrix);

	tgscale = girl.GetScaleFloat3();
	tgrot = girl.GetRotationFloat3();

	if (npcstate == 1)
	{
		if (npctimer.GetMilisecondsElapsed() >= 2000) {
			npctimer.Stop();
			uiflag = false;
			npcstate = 2;
			moneyui->Clear();
			moneyui->Show();

			//reset camera
			this->camera3d->cameratype = 0;
		}
	}

	if (npcstate == 3)
	{
		if (npctimer.GetMilisecondsElapsed() >= 2000 && !b_reachdestinationSign) {
			npctimer.Stop();
			b_reachdestinationSign = true;
		}
	}

	//update triggerbox pos
	if (starttrigger.collisionuse)
	{
		starttrigger.obb = starttrigger.originobb;
		auto girlpos = girl.GetPositionFloat3();
		auto girlrot = girl.GetRotationFloat3();
		auto girlscl = girl.GetScaleFloat3();
		auto costworldMatrix = XMMatrixScaling(girlscl.x, girlscl.y, girlscl.z)
			* XMMatrixRotationRollPitchYaw(girlrot.x, girlrot.y, girlrot.z)
			* XMMatrixTranslation(girlpos.x, girlpos.y, girlpos.z);
		starttrigger.obb.Transform(starttrigger.obb, costworldMatrix);
	}
	

	if (endtrigger.collisionuse)
	{
		endtrigger.obb = endtrigger.originobb;
		auto coedworldMatrix = XMMatrixScaling(tgscale.x, tgscale.y, tgscale.z)
			* XMMatrixRotationRollPitchYaw(tgrot.x, tgrot.y, tgrot.z)
			* XMMatrixTranslation(tgpos.x, tgpos.y, tgpos.z);
		endtrigger.obb.Transform(endtrigger.obb, coedworldMatrix);
	}
	

	//check
	if (endtrigger.collisionuse)
	{
		DirectX::ContainmentType coresult = camera3d->GetCameraCollision()->frustum.Contains(endtrigger.obb);
		if (coresult == 2 || coresult == 1)
			possignshowflag = true;
		else
			possignshowflag = false;
	}
	

	//pos sign pos 
	auto targetpos = XMLoadFloat3(&tgpos);
	auto camviewport = this->camera3d->viewport;
	auto vec = XMVector3Project(targetpos, camviewport.TopLeftX, camviewport.TopLeftY,
		camviewport.Width, camviewport.Height, camviewport.MinDepth, camviewport.MaxDepth,
		this->camera3d->GetProjectionMatrix(), this->camera3d->GetViewMatrix(), DirectX::XMMatrixIdentity());
	XMFLOAT3 co;
	XMStoreFloat3(&co, vec);
	co = XMFLOAT3(std::clamp(co.x, -200.0f, 800 + 100.0f), std::clamp(co.y, -200.0f, 600 + 100.0f) - 50, 0);
	possign.SetPosition(co);

	//dessign rot
	auto desdir = XMFLOAT2(tgpos.x - carpos.x,tgpos.z - carpos.z);
	float angle = atan2(desdir.x, desdir.y);
	desdirsign.SetRotation(XMFLOAT3(0, 0, angle));
}
//
void Npc::Draw3D(const XMMATRIX & viewProjectionMatrix) 
{
	if (npcstate <= 1)
	{
		girl.Draw(viewProjectionMatrix);
	}
	
}

void Npc::Draw2D(const XMMATRIX & viewProjectionMatrix2d) 
{
	if (possignshowflag && npcstate == 2)
		possign.Draw(viewProjectionMatrix2d);

	if (npcstate == 2 && !possignshowflag)
	{
		desdirsign.Draw(viewProjectionMatrix2d);
	}
	
	if (!b_reachdestinationSign && npcstate == 3)
	{
		npcUI_reach.Draw(viewProjectionMatrix2d);
	}
	
	if (uiflag)
		npcUI_thank.Draw(viewProjectionMatrix2d);
}


void Npc::Release() {
	
}
