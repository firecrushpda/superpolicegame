//=============================================================================
//
// [CarAIController.cpp]
// Author : GP11B183 28 àN —B‘½ (ƒpƒ“@ƒCƒ^)
//
//=============================================================================
#include "CarAIController.h"

CarAIController::CarAIController()
{
	mSpeed = 35.0f;
	mAvoidSpeed = 300.0f;
	mWayPointNum = 1;
	roadcontentfile = "SceneLayout.txt";
	roadcontentfile1 = "SceneLayout1.txt";
	roadcontentfile2 = "SceneLayout2.txt";
	possign_flag = true;

	LoadContent();
}


CarAIController::~CarAIController()
{

}

void CarAIController::MoveTowards(XMFLOAT3* pos, float delta)
{
	auto mTrackPoints = GetCurrentTrack();
	float epsilon = 1.0f;
	XMFLOAT3 dir;

	auto carpos = mAICar->carrender.GetPositionFloat3();
	auto carrot = mAICar->carrender.GetRotationFloat3();

	dir.x = pos->x - carpos.x;
	dir.y = pos->y - carpos.y;
	dir.z = pos->z - carpos.z;

	float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;


	carpos.x += dir.x * mSpeed * delta;
	carpos.z += dir.z * mSpeed * delta;

	float angle = atan2(carpos.x - pos->x, carpos.z - pos->z);
	carrot.y = angle;

	if (canmove)
	{
		mAICar->carrender.SetPosition(carpos);
		mAICar->carrender.SetRotation(carrot);
	}

	if (abs(pos->x - carpos.x) <= epsilon && abs(pos->y - carpos.y) <= epsilon)
	{
		if (mWayPointNum >= mTrackPoints.size() - 1)
		{
			mWayPointNum = 0;
			return;
		}
		else
		{
			mWayPointNum++;
			return;
		}
	}
}

void CarAIController::Update(float delta)
{
	auto mTrackPoints = GetCurrentTrack();
	MoveTowards(&mTrackPoints[mWayPointNum], delta);
	if (countdown > 0 && hasbeenfound)
	{
		countdown-= 1.0f / 60.0f;
	}
	if (b_cooldown)
	{
		cooldowmtimer += 1.0f / 60.0f;
		if (cooldowmtimer >= cooldown_waittime)
		{
			b_cooldown = false;
			cooldowmtimer = 0.0f;

			CatchCar();
		}
	}
}

void CarAIController::AddWaypoint(XMFLOAT3 pos)
{
	mTrackPoints.push_back(pos);
}


void CarAIController::Avoid(XMFLOAT3 pos, float delta)
{
	XMFLOAT3 dir;

	auto carpos = mAICar->carrender.GetPositionFloat3();
	auto carrot = mAICar->carrender.GetRotationFloat3();

	dir.x = pos.x - carpos.x;
	dir.y = pos.y - carpos.y;
	dir.z = pos.z - carpos.z;

	float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;


	carpos.x -= dir.x * mAvoidSpeed * delta;
	carpos.z -= dir.z * mAvoidSpeed * delta;

	mAICar->carrender.SetPosition(carpos);
	mAICar->carrender.SetRotation(carrot);
}

void CarAIController::ResetCarPosition()
{
	auto mTrackPoints = GetCurrentTrack();
	mAICar->carrender.SetPosition(mTrackPoints.at(0));
}

void CarAIController::CatchCar()
{
	currentcarindex++;
	if (currentcarindex > 2) {
		currentcarindex = 0;
	}
	possign_flag = true;
	canmove = true;
	SetAiCarByIndex();
}

void  CarAIController::LoadContent() 
{
	std::ifstream inFile;
	inFile.open(roadcontentfile);

	if (!inFile.good())
	{
		std::cerr << "ERROR: Cannot find Road Content file\n";
		return;
	}
	else {
		std::cerr << "Scene layout File found\n";

		std::string input;
		while (!inFile.eof()) {
			inFile >> input;

			if (input.compare("O") == 0)
			{
				float x, y, z;

				inFile >> x;
				inFile >> y;
				inFile >> z;
				this->AddWaypoint(XMFLOAT3(x, y, z));
			}
		}
	}

	std::ifstream inFile2;
	inFile2.open(roadcontentfile2);

	if (!inFile2.good())
	{
		std::cerr << "ERROR: Cannot find Road Content file\n";
		return;
	}
	else {
		std::cerr << "Scene layout File found\n";

		std::string input;
		while (!inFile2.eof()) {
			inFile2 >> input;

			if (input.compare("O") == 0)
			{
				float x, y, z;

				inFile2 >> x;
				inFile2 >> y;
				inFile2 >> z;
				mTrackPoints2.push_back(XMFLOAT3(x, y, z));
			}
		}
	}

	std::ifstream inFile1;
	inFile1.open(roadcontentfile1);

	if (!inFile1.good())
	{
		std::cerr << "ERROR: Cannot find Road Content file\n";
		return;
	}
	else {
		std::cerr << "Scene layout File found\n";

		std::string input;
		while (!inFile1.eof()) {
			inFile1 >> input;

			if (input.compare("O") == 0)
			{
				float x, y, z;

				inFile1 >> x;
				inFile1 >> y;
				inFile1 >> z;
				mTrackPoints1.push_back(XMFLOAT3(x, y, z));
				//this->AddWaypoint(XMFLOAT3(x, y, z));
			}
		}
	}
}

void CarAIController::CarEscape() {
	hasbeenfound = false;
	countdown = 60;
	b_cooldown = true;
	cooldowmtimer = 0.0f;
	possign_flag = false;

	mAICar->carrender.b_modelview = false;
	mAICar->carrender.b_use = false;
	mAICar->carrender.GetCollisionObject()->collisionuse = false;
}

void CarAIController::ResetCACStatus() 
{
	mWayPointNum = 1;
	countdown = 60;
	b_cooldown = true;
}

void CarAIController::SetAiCarByIndex() {
	if (currentcarindex == 0)
	{
		SetAICar(enemy);
	}
	else if (currentcarindex == 1)
	{
		SetAICar(enemy1);
	}
	else if (currentcarindex == 2)
	{
		SetAICar(enemy2);
	}
}

