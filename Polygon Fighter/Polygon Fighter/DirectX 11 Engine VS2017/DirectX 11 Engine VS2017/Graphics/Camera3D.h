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

	const XMMATRIX & GetViewMatrix() const;
	const XMMATRIX & GetProjectionMatrix() const;

	CollisionCamera* GetCameraCollision();

	float focusLength = 50;
	unsigned int cameratype = 0;//0 follow  1 free
	GameObject3D* focusgo;
	XMFLOAT3 roundviewrot;
	D3D11_VIEWPORT viewport;

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