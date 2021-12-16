#include "Car.h"


bool Car::CarInitialize(const std::string & filePath, ID3D11Device * device,
						ID3D11DeviceContext * deviceContext, 
						ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader,bool isCharacter) 
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;
	this->isCharacter = isCharacter;

	carrender.Initialize(filePath, device, deviceContext, cb_vs_vertexshader);
	carrender.SetGlobalMatirx(DirectX::XMMatrixIdentity()); 
	if (this->isCharacter)
	{
		taxirender.Initialize("Data\\Objects\\test\\taxi.obj", device, deviceContext, cb_vs_vertexshader);
		auto gmatrix = XMMatrixSet(0, 0, 1, 0,
									0, 1, 0, 0,
									-1, 0, 0, 0,
									0, -1, 0, 1)*
						XMMatrixSet(0, 0, 1, 0,
									0, 1, 0, 0,
									-1, 0, 0, 0,
									0, 0, 0, 1) ;
		taxirender.SetGlobalMatirx(gmatrix);
		taxirender.SetCollisionBoxView(false);
		//taxirender.SetScale(0.2, 0.2, 0.2);
	}
		
	
	mMaxSpeed = 1.5f;
	mCarMaxSpeed = XMFLOAT3(0.03f, 0.0f, 1.5f); // Steering, Up, Fowards//XMFLOAT3(0.015f, 0.0f, 8.0f)
	mCarSpeed = 0.0f;
	mCarVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mCarAcceleration = XMFLOAT3(0.001f, 1.0f, 0.03f); // Steering, Up, FowardsXMFLOAT3(0.04f, 1.0f, 0.01f)

	mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mFrictionForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEngineForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mBrakeForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mDragforce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mGravity = XMFLOAT3(0.0f, -9.81f, 0.0f);

	mForces[0] = mFrictionForce;
	mForces[1] = mEngineForce;
	mForces[2] = mBrakeForce;

	mPhysicsMode = PHYSICS_NONE;

	mNetForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mSlideForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mForce.push_back(&mFrictionForce);
	mForce.push_back(&mEngineForce);
	mForce.push_back(&mBrakeForce);
	mForce.push_back(&mSlideForce);
	mForce.push_back(&mDragforce);
	mCollsionOn = false;
	_mass = 1.0f;

	return true;
}

void Car::Update(float delta_time, const XMMATRIX & viewProjectionMatrix) 
{
	if (isCharacter)
	{
		auto trans = actor->getGlobalPose();
		float angle; PxVec3 rot;
		trans.q.toRadiansAndUnitAxis(angle, rot);

		auto xmrot = XMFLOAT3(rot.x * angle, rot.y * angle, rot.z * angle);
		xmrot = Adjustrot(xmrot);
		auto xmpos = XMFLOAT3(trans.p.x, trans.p.y, trans.p.z);
		auto xmstrot = XMLoadFloat3(&xmrot);
		auto xmstpos = XMLoadFloat3(&xmpos);
		if (CanUseVector(xmstrot)) {
			carrender.SetRotation(xmrot);
			taxirender.SetRotation(xmrot);
		}
		
		if (CanUseVector(xmstpos)) {
			carrender.SetPosition(xmpos);
			taxirender.SetPosition(xmpos);
		}
		taxirender.Update(delta_time, viewProjectionMatrix);
		carrender.Update(delta_time, viewProjectionMatrix);
	}
	else
	{
		carrender.Update(delta_time, viewProjectionMatrix);
	}
	

	//update car distance for score
	cardistance += std::abs(GetCarVelocity()) * 1.0f / 60.0f;

	//dissolve animation
	if (inDissolveProcess) {
		if (dstate == dissolveState::tocar)
		{
			taxidrate += frate;
			cardrate -= frate;
			taxidrate = std::clamp(taxidrate, 0.0f, 1.0f);
			cardrate = std::clamp(cardrate, 0.0f, 1.0f);
			if (cardrate <= 0.0f) 
			{
				taxidrate = 1;
				inDissolveProcess = false;
			}
		}
		else if (dstate == dissolveState::totaxi)
		{
			taxidrate -= frate;
			cardrate += frate;
			taxidrate = std::clamp(taxidrate, 0.0f, 1.0f);
			cardrate = std::clamp(cardrate, 0.0f, 1.0f);
			if (taxidrate <= 0.0f)
			{
				cardrate = 1;
				inDissolveProcess = false;
			}
		}
	}
}

void Car::Draw( const XMMATRIX & viewProjectionMatrix, ConstantBuffer<CB_PS_IBLSTATUS>& cbps_iblstatus)
{
	cbps_iblstatus.data.dissolveThreshold = cardrate;
	cbps_iblstatus.ApplyChanges();
	carrender.Draw(viewProjectionMatrix);

	if (isCharacter) {
		cbps_iblstatus.data.dissolveThreshold = taxidrate;
		cbps_iblstatus.ApplyChanges();
		taxirender.Draw(viewProjectionMatrix);

		if (carbar_drawflag)
			carbar.Draw(viewProjectionMatrix);
	}

	//deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//for (size_t i = 0; i <  carrender.GetCollisionObject()->debugmesh.size(); i++)
	//{
	//	auto carscl = carrender.GetScaleFloat3();
	//	auto carrot = carrender.GetRotationFloat3();
	//	auto carpos = carrender.GetPositionFloat3();
	//	auto coobb = carrender.GetCollisionObject()->originobb;
	//	auto coworldMatrix = XMMatrixScaling(carscl.x, carscl.y, carscl.z)
	//		* XMMatrixRotationRollPitchYaw(carrot.x, carrot.y, carrot.z)
	//		* XMMatrixTranslation(carpos.x + mCarVelocity.z * sin(carrot.y), carpos.y, carpos.z + mCarVelocity.z * cos(carrot.y));
	//	coobb.Transform(coobb, coworldMatrix);
	//	this->cb_vs_vertexshader->data.wvpMatrix = coworldMatrix * viewProjectionMatrix;//offsetmat
	//	this->cb_vs_vertexshader->data.worldMatrix = coworldMatrix;//offsetmat
	//	this->cb_vs_vertexshader->ApplyChanges();

	//	//carrender.GetCollisionObject()->debugmesh.at(i).Draw();
	//}

	cbps_iblstatus.data.dissolveThreshold = 0;
	cbps_iblstatus.ApplyChanges();
}

void Car::UpdateForce() 
{
	mNetForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (std::vector<XMFLOAT3*>::iterator i = mForce.begin(); i != mForce.end(); i++)
	{
		mNetForce.x += (*i)->x;
		mNetForce.y += (*i)->y;
		mNetForce.z += (*i)->z;
	}
}

void Car::MoveFowards(float delta, float accelfactor,std::vector<RenderableGameObject*> mapgo)
{
	mCarVelocity.z += ((accelfactor * mCarMaxSpeed.z) - mCarVelocity.z) * mCarAcceleration.z * delta;

	XMFLOAT3 dir = carrender.GetPositionFloat3();
	XMFLOAT3 rot = carrender.GetRotationFloat3();

	//collision check
	bool canmove = true;
	//auto carscl = carrender.GetScaleFloat3();
	//auto carrot = carrender.GetRotationFloat3();
	//auto carpos = carrender.GetPositionFloat3();
	//auto coobb = carrender.GetCollisionObject()->originobb;
	//auto coworldMatrix = XMMatrixScaling(carscl.x, carscl.y, carscl.z)
	//	* XMMatrixRotationRollPitchYaw(carrot.x , carrot.y, carrot.z )
	//	* XMMatrixTranslation(carpos.x + mCarVelocity.z * sin(carrot.y), carpos.y, carpos.z + mCarVelocity.z * cos(carrot.y));
	//coobb.Transform(coobb, coworldMatrix);
	//for (size_t i = 0; i < mapgo.size(); i++)
	//{
	//	if (mapgo.at(i)->b_modelview)
	//	{
	//		auto obb = mapgo.at(i)->GetCollisionObject()->obb;
	//		DirectX::ContainmentType coresult = coobb.Contains(obb);
	//		if (coresult == 2 || coresult == 1)
	//		{
	//			m_Sound->PlayIndexSound(Sound::SOUND_LABEL_SE_syoutotu);
	//			canmove = false;
	//			break;
	//		}
	//	}
	//}
	
	if (canmove)
	{
		mCollsionOn = true;
		dir.x += mCarVelocity.z * sin(rot.y);
		dir.z += mCarVelocity.z * cos(rot.y);
	}
	else
	{
		mCollsionOn = false;
		mCarVelocity.z = 0;
	}

	carrender.SetPosition(dir);
	taxirender.SetPosition(dir);
}

float Car::CalcWheelSpeed(float delta)
{
	float radius = 0.05f;
	float wheelAngle = 0.0f;
	wheelAngle += XM_PI / radius / 360 * delta;
	wheelAngle *= mCarVelocity.z;
	return wheelAngle * 10;
}

void Car::Turn(float delta, float accelfactor)
{
	auto rot = carrender.GetRotationFloat3();
	auto carbarrot = carbar.GetRotationFloat3();
	mCarVelocity.x += ((accelfactor * mCarMaxSpeed.x) - mCarVelocity.x) * mCarAcceleration.z * delta;
	if (mCarVelocity.z > 0.1)
	{
		rot.y += mCarVelocity.x;
		carbar.SetRotation(carbarrot.x, carbarrot.y, carbarrot.z - mCarVelocity.x);
	}
	else if (mCarVelocity.z < -0.1)
	{
		rot.y -= mCarVelocity.x;
		carbar.SetRotation(carbarrot.x, carbarrot.y, carbarrot.z + mCarVelocity.x);
	}
	carrender.SetRotation(rot);
	taxirender.SetRotation(rot);

}


float Car::GetCarVelocity() 
{
	return mCarVelocity.z;
}

float Car::GetMaxSpeed() 
{
	return mMaxSpeed;
}

void Car::StartDissolveAnimaion()
{
	if (inDissolveProcess) return;

	inDissolveProcess = true;
	taxidrate == 0.0f ? dstate = dissolveState::tocar : dstate = dissolveState::totaxi;
}

bool Car::CanUseVector(XMVECTOR vec) {
	bool canuse = true;
	auto nan = XMVectorIsNaN(vec);
	auto infin = XMVectorIsInfinite(vec);
	if (XMVectorGetX(XMVectorNotEqual(nan, XMVectorFalseInt())) == true
		|| XMVectorGetX(XMVectorNotEqual(infin, XMVectorFalseInt())) == true)
	{
		canuse = false;
	}
	return canuse;
}

XMFLOAT3 Car::Adjustrot(XMFLOAT3 vec)
{
	XMFLOAT3 rtn = vec;
	if (rtn.x >= XM_2PI)
		rtn.x -= 2 * XM_2PI;
	if (rtn.x < -XM_2PI)
		rtn.x += 2 * XM_2PI;
	if (rtn.y >= XM_2PI)
		rtn.y -= 2 * XM_2PI;
	if (rtn.y < -XM_2PI)
		rtn.y += 2 * XM_2PI;
	if (rtn.z >= XM_2PI)
		rtn.z -= 2 * XM_2PI;
	if (rtn.z < -XM_2PI)
		rtn.z += 2 * XM_2PI;
	return rtn;
}