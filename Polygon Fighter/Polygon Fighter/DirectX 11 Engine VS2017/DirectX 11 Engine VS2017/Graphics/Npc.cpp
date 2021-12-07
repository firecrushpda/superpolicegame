#include "Npc.h"

bool Npc::Init(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext,
	ConstantBuffer<CB_VS_vertexshader>& cb_vs_vertexshader, ConstantBuffer<CB_VS_vertexshader_2d> & cb_vs_vertexshader_2d, Camera3D* camera3d)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->camera3d = camera3d;

	npctimer.Start();

	if (!girl.Initialize(filePath, device, deviceContext, cb_vs_vertexshader))
		return false;
	girl.SetPosition(0, 1, 0);
	girl.SetScale(0.005, 0.005, 0.005);
	girl.SetRotation(XMFLOAT3(-XM_PI / 2, 0, 0));
	//girl.SetGlobalMatirx(XMMatrixRotationRollPitchYaw(0, XM_PI / 2, 0));
	girl.SetCollisionBoxView(false);

	//sprite
	npcUI_thank.Initialize(device, deviceContext, 400, 200, "Data\\Textures\\npcUIthank.png", cb_vs_vertexshader_2d);
	npcUI_thank.SetPosition(XMFLOAT3(200, 400, 0));
	
	//pos sign
	possign.Initialize(device, deviceContext, 75, 77, "Data\\Textures\\marker01.png", cb_vs_vertexshader_2d);

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

void Npc::Update(float dt, const XMMATRIX & viewProjectionMatrix)
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
	
	if (uiflag)
		npcUI_thank.Draw(viewProjectionMatrix2d);
}


void Npc::Release() {
	
}
