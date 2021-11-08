
#pragma once
#include <DirectXCollision.h>
#include <vector>
#include "Vertex.h"
#include "Camera3D.h"


struct Ray
{
	Ray();
	Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

	static Ray ScreenToRay(const Camera3D& camera, float screenX, float screenY);

	bool Hit(const DirectX::BoundingBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool Hit(const DirectX::BoundingOrientedBox& box, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool Hit(const DirectX::BoundingSphere& sphere, float* pOutDist = nullptr, float maxDist = FLT_MAX);
	bool XM_CALLCONV Hit(DirectX::FXMVECTOR V0, DirectX::FXMVECTOR V1, DirectX::FXMVECTOR V2, float* pOutDist = nullptr, float maxDist = FLT_MAX);

	DirectX::XMFLOAT3 origin;		
	DirectX::XMFLOAT3 direction;	
};


class Collision
{
public:

	struct WireFrameData
	{
		std::vector<Vertex3D> vertexVec;
		std::vector<WORD> indexVec;					
	};


	static WireFrameData CreateBoundingBox(const DirectX::BoundingBox& box, const DirectX::XMFLOAT4& color);

	static WireFrameData CreateBoundingOrientedBox(const DirectX::BoundingOrientedBox& box, const DirectX::XMFLOAT4& color);

	static WireFrameData CreateBoundingSphere(const DirectX::BoundingSphere& sphere, const DirectX::XMFLOAT4& color, int slices = 20);

	static WireFrameData CreateBoundingFrustum(const DirectX::BoundingFrustum& frustum, const DirectX::XMFLOAT4& color);


	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);

	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling2(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);

	static std::vector<DirectX::XMMATRIX> XM_CALLCONV FrustumCulling3(
		const std::vector<DirectX::XMMATRIX>& Matrices, const DirectX::BoundingBox& localBox, DirectX::FXMMATRIX View, DirectX::CXMMATRIX Proj);

private:
	static WireFrameData CreateFromCorners(const DirectX::XMFLOAT3(&corners)[8], const DirectX::XMFLOAT4& color);
};

