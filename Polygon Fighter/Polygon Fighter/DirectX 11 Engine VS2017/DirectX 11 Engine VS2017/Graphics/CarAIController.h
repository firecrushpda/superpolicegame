//=============================================================================
//
// [CarAIController.h]
// Author : GP11B183 28 ‡N óBëΩ (ÉpÉìÅ@ÉCÉ^)
//
//=============================================================================
#pragma once
#pragma once

#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include "Car.h"

using namespace std;
using namespace DirectX;

class CarAIController
{
public:
	CarAIController();
	~CarAIController();

	unsigned int currentcarindex = 0;

	bool hasbeenfound = false;
	bool possign_flag = true;
	Sprite possign;
	float countdown = 60;
	bool canmove = false;
	bool b_cooldown = false;

	Car* enemy;
	Car* enemy1;
	Car* enemy2;

	float cooldowmtimer;
	float cooldown_waittime = 15;
	//Timer cooldowmtimer;

	void Update(float delta);

	void SetAICar(Car* aicar) { 
		mAICar = aicar;
		mWayPointNum = 1;
		aicar->carrender.b_use = true;
		aicar->carrender.b_modelview = true;
		aicar->carrender.GetCollisionObject()->collisionuse = true;
		auto pos = GetCurrentTrack().at(0);
		aicar->carrender.SetPosition(pos);
	}
	Car* mAICar;
	void SetPLayerCarPos(XMFLOAT3* pos) { mPlayerCar = pos; }
	void MoveTowards(XMFLOAT3* pos, float delta);
	void AddWaypoint(XMFLOAT3 pos);
	void Avoid(XMFLOAT3 pos, float delta);

	void ResetCarPosition();

	void CatchCar();

	vector<XMFLOAT3> GetCurrentTrack() {
		if (currentcarindex == 0)
		{
			return mTrackPoints;
		}
		else if (currentcarindex == 1) 
		{
			return mTrackPoints1;
		}
		else if (currentcarindex == 2) 
		{
			return mTrackPoints2;
		}
	}

	void SetAiCarByIndex();

	void CarEscape();

	void ResetCACStatus();
private:

	vector<XMFLOAT3> mTrackPoints;
	vector<XMFLOAT3> mTrackPoints1;
	vector<XMFLOAT3> mTrackPoints2;
	XMFLOAT3* mPlayerCar;

	float mSpeed;
	XMFLOAT3* mCurrentWaypoint;
	int mWayPointNum;
	float mAvoidSpeed;



	std::string roadcontentfile;
	std::string roadcontentfile1;
	std::string roadcontentfile2;
	void LoadContent();
};


