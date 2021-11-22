#pragma once
#include "RenderableGameObject.h"
#include "CarSpeedUI.h"

enum PhysicsMode
{
	PHYSICS_NONE,
	PHYSICS_VELOCITY,
	PHYSICS_ACCELERATION,
	PHYSICS_MULTIFORCE,
	PHYSICS_SLIDING
};

class Car
{
public:
	//renderobject
	RenderableGameObject carrender;

	RenderableGameObject carbar;

	//rotatespeed
	float rotatespeed = 0.001f;

	//acceleration
	bool accflag = false;
	float acc = 0;
	float accincrease = 0.5f;
	float hoizontalacc = 0;
	float verticalacc = 0;
	float accmax = 100;

	bool mCollsionOn;

	bool CarInitialize(const std::string & filePath, ID3D11Device * device, ID3D11DeviceContext * deviceContext, ConstantBuffer<CB_VS_vertexshader> & cb_vs_vertexshader);
	void Update(float delta_time, const XMMATRIX & viewProjectionMatrix);
	void Draw(const XMMATRIX & viewProjectionMatrix);

	void MoveFowards(float delta, float accelfactor);
	float CalcWheelSpeed(float delta);
	void Turn(float delta, float accelfactor);

	float GetCarVelocity();
	float GetMaxSpeed();

	CarSpeedUI carsui;
	bool warninguiflag;
	Sprite warningui;
	bool carbar_drawflag = false;
	bool haspassenger = false;

	XMFLOAT3 mCarAcceleration;
	XMFLOAT3 mCarMaxSpeed;

	float cardistance = 0;
	
private:

	float mWheelRadius;
	float mEngineSpeed;
	float mCarSpeed;
	float mMaxSpeed;
	XMFLOAT3 mCarVelocity;

	//test
	PhysicsMode mPhysicsMode;

	//Forces
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
	XMFLOAT3 mFrictionForce;
	XMFLOAT3 mBrakeForce;
	XMFLOAT3 mEngineForce;
	XMFLOAT3 mNetForce;
	XMFLOAT3 mDragforce; // add to net force
	XMFLOAT3 mGravity;


	float mTheTa;
	float mFrCoef;
	float mForceMag;
	XMFLOAT3 mSlideForce;

	std::vector<XMFLOAT3*> mForce;
	XMFLOAT3 mForces[3];
	bool mLaminar;
	float _mass;

	//D3DX refer
	ID3D11Device * device = nullptr;
	ID3D11DeviceContext * deviceContext = nullptr;
	ConstantBuffer<CB_VS_vertexshader> * cb_vs_vertexshader = nullptr;

	void UpdateForce();
	
};

