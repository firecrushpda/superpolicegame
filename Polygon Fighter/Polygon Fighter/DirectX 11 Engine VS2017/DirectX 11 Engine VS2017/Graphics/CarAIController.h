//=============================================================================
//
// [CarAIController.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
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

	void Update(float delta);

	void SetAICar(Car* aicar) { mAICar = aicar; }
	void SetPLayerCarPos(XMFLOAT3* pos) { mPlayerCar = pos; }
	void MoveTowards(XMFLOAT3* pos, float delta);
	void AddWaypoint(XMFLOAT3 pos);
	void Avoid(XMFLOAT3 pos, float delta);

private:
	vector<XMFLOAT3> mTrackPoints;
	XMFLOAT3* mPlayerCar;
	Car* mAICar;
	float mSpeed;
	XMFLOAT3* mCurrentWaypoint;
	int mWayPointNum;
	float mAvoidSpeed;

	std::string roadcontentfile;
	void LoadContent();
};


