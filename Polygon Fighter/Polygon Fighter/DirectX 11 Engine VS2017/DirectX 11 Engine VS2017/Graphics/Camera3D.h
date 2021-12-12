//=============================================================================
//
// 3Dカメラ処理 [Camera3D.h]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#pragma once
#include "GameObject3D.h"
using namespace DirectX;

class Camera3D : public GameObject3D
{
public:
	Camera3D();

	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);
	void ChangeFocusMode(unsigned int index, GameObject3D* go);
	void ResetFollowCamera();

	void LoadCameraWorks_Point();
	void LoadCameraWorks_Line();
	void LoadCameraWorks_Rotate();

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	void DeepCopyTo(Camera3D* camera3d);

	CollisionCamera* GetCameraCollision();

	float focusLength = 50;
	unsigned int cameratype = 0;//0 follow  1 free
	GameObject3D* focusgo;
	XMFLOAT3 roundviewrot;
	D3D11_VIEWPORT viewport;

	std::string focusname = "maincharactercar"; //bigsite
	std::vector<XMFLOAT3>mCameraWorkTrack_Point;
	std::vector<XMFLOAT3>mCameraWorkTrack_Line;
	std::vector<XMFLOAT3>mCameraWorkTrack_Rotate;
	unsigned int cwpointindex = 0;
	unsigned int cwlineindex = 0;
	unsigned int cwrotateindex = 0;
	
	Timer timer;
	float cwlineSpeed = 20.0f;
	float cwwaittime = 3.0f;
	float cwrotateheight = 5;
	float cwrotatedistance = 10;
	float cwrotatespeed = 0.01;

	float cf_fheight = 0.0f;
	float cf_front = 3.0f;
	float cf_back = 0.8;//
	float cf_height = 0.45;//
private:
	void UpdateMatrix() override;
	
	//collision
	CollisionCamera* collision = nullptr;

	bool ProcessCollsion(CollsionType cotype);
	
	XMMATRIX viewMatrix;//ビューマトリクス
	XMMATRIX projectionMatrix;//プロジェクトマトリクス

};