#include "Car.h"

bool Car::CarInitialize(const std::string & filePath, ID3D11Device * device,
						ID3D11DeviceContext * deviceContext, 
						ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader) 
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->cb_vs_vertexshader = &cb_vs_vertexshader;

	carrender.Initialize(filePath, device, deviceContext, cb_vs_vertexshader);
	carrender.SetGlobalMatirx(DirectX::XMMatrixIdentity()); 

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
	mCollsionOn = true;
	_mass = 1.0f;

	return true;
}

void Car::Update(float delta_time, const XMMATRIX & viewProjectionMatrix) 
{
	XMFLOAT3 temp = carrender.GetPositionFloat3();
	temp.x += mGravity.x * _mass * delta_time;
	temp.y += mGravity.y * _mass * delta_time;
	temp.z += mGravity.z * _mass * delta_time;

	if (temp.y < 1.0f)
	{
		temp.y = 1.0f;
	}

	carrender.SetPosition(temp);

	UpdateForce();
	carrender.Update(delta_time, viewProjectionMatrix);

	//update car distance
	cardistance += GetCarVelocity() * 1.0f / 60.0f;
}

void Car::Draw( const XMMATRIX & viewProjectionMatrix)
{
	carrender.Draw(viewProjectionMatrix);
	if (carbar_drawflag)
		carbar.Draw(viewProjectionMatrix);
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

void Car::MoveFowards(float delta, float accelfactor)
{
	mCarVelocity.z += ((accelfactor * mCarMaxSpeed.z) - mCarVelocity.z) * mCarAcceleration.z * delta;

	XMFLOAT3 dir = carrender.GetPositionFloat3();
	XMFLOAT3 rot = carrender.GetRotationFloat3();

	dir.x += mCarVelocity.z * sin(rot.y);
	dir.z += mCarVelocity.z * cos(rot.y);

	carrender.SetPosition(dir);
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


}

float Car::GetCarVelocity() 
{
	return mCarVelocity.z;
}

float Car::GetMaxSpeed() 
{
	return mMaxSpeed;
}