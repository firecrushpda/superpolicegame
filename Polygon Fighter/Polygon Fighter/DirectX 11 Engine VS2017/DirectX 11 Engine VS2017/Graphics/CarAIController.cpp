//=============================================================================
//
// [CarAIController.cpp]
// Author : GP11B183 28 àN —B‘½ (ƒpƒ“@ƒCƒ^)
//
//=============================================================================
#include "CarAIController.h"

CarAIController::CarAIController()
{
	mSpeed = 30.0f;
	mAvoidSpeed = 300.0f;
	mWayPointNum = 0;
	roadcontentfile = "SceneLayout.txt";

	LoadContent();

}


CarAIController::~CarAIController()
{

}

void CarAIController::MoveTowards(XMFLOAT3* pos, float delta)
{
	float epsilon = 0.2f;
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

	mAICar->carrender.SetPosition(carpos);
	mAICar->carrender.SetRotation(carrot);

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
	MoveTowards(&mTrackPoints[mWayPointNum], delta);
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
}

