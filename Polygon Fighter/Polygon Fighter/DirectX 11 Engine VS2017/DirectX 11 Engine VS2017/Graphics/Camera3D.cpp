//=============================================================================
//
// 3Dカメラ処理 [Camera3D.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Camera3D.h"

//=============================================================================
//初期化
//=============================================================================
Camera3D::Camera3D()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	if (collision == nullptr)
		ProcessCollsion(CollsionType::Camera);
	this->UpdateMatrix();

	LoadCameraWorks_Point();
	LoadCameraWorks_Line();
	//LoadCameraWorks_Line();
	timer.Start();
}

//=============================================================================
// プロジェクションマトリクス設置関数
//=============================================================================
void Camera3D::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);

	collision->orifrustum = BoundingFrustum(this->projectionMatrix);
	collision->frustum = collision->orifrustum;
}

//=============================================================================
// ビューマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera3D::GetViewMatrix() const
{
	return this->viewMatrix;
}

//=============================================================================
// プロジェクションマトリクス取る関数
//=============================================================================
const XMMATRIX & Camera3D::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

void Camera3D::DeepCopyTo(Camera3D* camera3d)
{
	camera3d->focusLength = this->focusLength;
	camera3d->cameratype = this->cameratype;
	//camera3d->focusgo = ;
	camera3d->roundviewrot = this->roundviewrot;
	camera3d->viewport = this->viewport;

	camera3d->cf_fheight = this->cf_fheight;
	camera3d->cf_front = this->cf_front;
	camera3d->cf_back = this->cf_back;//
	camera3d->cf_height = this->cf_height;//

	camera3d->collision = new CollisionCamera();
	camera3d->collision->collisionuse = this->collision->collisionuse;
	camera3d->collision->oritransform = this->collision->oritransform;
	camera3d->collision->frustum = this->collision->frustum;
	camera3d->collision->orifrustum = this->collision->orifrustum;
	camera3d->collision->ct = this->collision->ct;
}

//=============================================================================
// マトリクス更新関数
//=============================================================================
void Camera3D::UpdateMatrix() //Updates view matrix and also updates the movement vectors
{
	//Calculate Camera3D rotation matrix
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//Calculate unit vector of cam target based off Camera3D forward value transformed by cam rotation matrix
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	//Adjust cam target to be offset by the Camera3D's current position
	camTarget += this->posVector;
	//Calculate up direction based on current rotation
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//Rebuild view matrix
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	//collision
	auto worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z)*
			XMMatrixTranslation(pos.x, pos.y, pos.z);

	collision->frustum = collision->orifrustum;
	collision->frustum.Transform(collision->frustum, worldMatrix);

	this->UpdateDirectionVectors();
}

void Camera3D::ChangeFocusMode(unsigned int index,GameObject3D* go) 
{
	cameratype = index;
	if (go != nullptr) focusgo = go;

	if (index == 0)
	{
		ResetFollowCamera();
	}
}

void Camera3D::ResetFollowCamera() {
	if (focusgo == nullptr)return;

	this->roundviewrot = focusgo->GetRotationFloat3();
	auto testpos = focusgo->GetPositionVector() + focusgo->GetBackwardVector() * 10;
	DirectX::XMFLOAT3 temp;
	DirectX::XMStoreFloat3(&temp, testpos);
	temp = DirectX::XMFLOAT3(temp.x, temp.y + 10, temp.z);
	SetPosition(temp);
	SetLookAtPos(focusgo->GetPositionFloat3());
}

void Camera3D::LoadCameraWorks_Point()
{
	std::ifstream inFile;
	inFile.open("CameraWorks_Point.txt");

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
				mCameraWorkTrack_Point.push_back(XMFLOAT3(x, y, z));
			}
		}
	}
}

void Camera3D::LoadCameraWorks_Line()
{
	std::ifstream inFile;
	inFile.open("CameraWorks_Line.txt");

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
				mCameraWorkTrack_Line.push_back(XMFLOAT3(x, y, z));
			}
		}
	}
}

void Camera3D::LoadCameraWorks_Rotate()
{
	std::ifstream inFile;
	inFile.open("CameraWorks_Rotate.txt");

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
				mCameraWorkTrack_Rotate.push_back(XMFLOAT3(x, y, z));
			}
		}
	}
	
}

bool Camera3D::ProcessCollsion(CollsionType cotype) 
{
	collision = new CollisionCamera();
	collision->ct = cotype;
	collision->collisionuse = true;
	
	collision->orifrustum = BoundingFrustum(this->projectionMatrix);
	collision->frustum = collision->orifrustum;
	return true;
}

CollisionCamera* Camera3D::GetCameraCollision() {
	return this->collision;
}
