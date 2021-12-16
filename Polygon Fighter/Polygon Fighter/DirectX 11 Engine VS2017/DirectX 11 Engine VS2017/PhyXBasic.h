#pragma once
#include <ctype.h>
#include "PxPhysicsAPI.h"
//
#include "vehicle/PxVehicleUtil.h"


#include "PhysXvehicle/snippetvehiclecommon/SnippetVehicleSceneQuery.h"
#include "PhysXvehicle/snippetvehiclecommon/SnippetVehicleFilterShader.h"
#include "PhysXvehicle/snippetvehiclecommon/SnippetVehicleTireFriction.h"
#include "PhysXvehicle/snippetvehiclecommon/SnippetVehicleCreate.h"

#include "PhysXvehicle/snippetcommon/SnippetPrint.h"
#include "PhysXvehicle/snippetcommon/SnippetPVD.h"
#include "PhysXvehicle/snippetutils/SnippetUtils.h"

#include "Model.h"
#include "Graphics/Mesh.h"

using namespace physx;
using namespace snippetvehicle;
using namespace DirectX;

class PhysXBasic
{
public:
	void initPhysics();

	VehicleDesc initVehicleDesc();

	void startBrakeMode();

	void stepPhysics();

	void incrementDrivingMode(const PxF32 timestep);

	void releaseAllControls();

	void startAccelerateForwardsMode();

	void startAccelerateForwardsSecond();

	void startAccelerateReverseMode();

	void startTurnHardLeftMode();

	void startTurnHardRightMode();

	void startHandbrakeTurnLeftMode();

	void startHandbrakeTurnRightMode();

	DirectX::XMMATRIX GetMatrixFromPxMatrix(PxMat44 mat);

	PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable;

	PxDefaultAllocator		gAllocator;
	PxDefaultErrorCallback	gErrorCallback;

	PxFoundation*			gFoundation = NULL;
	PxPhysics*				gPhysics = NULL;

	PxDefaultCpuDispatcher*	gDispatcher = NULL;
	PxScene*				gScene = NULL;

	PxCooking*				gCooking = NULL;

	PxMaterial*				gMaterial = NULL;

	PxPvd*                  gPvd = NULL;

	VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;
	PxBatchQuery*			gBatchQuery = NULL;

	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

	PxRigidStatic*			gGroundPlane = NULL;
	PxVehicleDrive4W*		gVehicle4W = NULL;

	bool					gIsVehicleInAir = true;

	PxF32					gVehicleModeLifetime = 4.0f;
	PxF32					gVehicleModeTimer = 0.0f;
	PxU32					gVehicleOrderProgress = 0;
	bool					gVehicleOrderComplete = false;
	bool					gMimicKeyInputs = false;

	PxVehicleDrive4WRawInputData gVehicleInputData;

	enum DriveMode
	{
		eDRIVE_MODE_ACCEL_FORWARDS = 0,
		eDRIVE_MODE_ACCEL_REVERSE,
		eDRIVE_MODE_HARD_TURN_LEFT,
		eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
		eDRIVE_MODE_HARD_TURN_RIGHT,
		eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
		eDRIVE_MODE_BRAKE,
		eDRIVE_MODE_NONE
	};

	DriveMode gDriveModeOrder[14] =
	{
		eDRIVE_MODE_BRAKE,
		eDRIVE_MODE_ACCEL_FORWARDS,
		eDRIVE_MODE_BRAKE,
		eDRIVE_MODE_ACCEL_REVERSE,
		eDRIVE_MODE_BRAKE,
		eDRIVE_MODE_HARD_TURN_LEFT,
		eDRIVE_MODE_BRAKE,
		eDRIVE_MODE_HARD_TURN_RIGHT,
		eDRIVE_MODE_ACCEL_FORWARDS,
		eDRIVE_MODE_HANDBRAKE_TURN_LEFT,
		eDRIVE_MODE_ACCEL_FORWARDS,
		eDRIVE_MODE_HANDBRAKE_TURN_RIGHT,
		eDRIVE_MODE_NONE
	};

	PxVehiclePadSmoothingData gPadSmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	PxVehicleKeySmoothingData gKeySmoothingData =
	{
		{
			6.0f,	//rise rate eANALOG_INPUT_ACCEL
			6.0f,	//rise rate eANALOG_INPUT_BRAKE		
			6.0f,	//rise rate eANALOG_INPUT_HANDBRAKE	
			2.5f,	//rise rate eANALOG_INPUT_STEER_LEFT
			2.5f,	//rise rate eANALOG_INPUT_STEER_RIGHT
		},
		{
			10.0f,	//fall rate eANALOG_INPUT_ACCEL
			10.0f,	//fall rate eANALOG_INPUT_BRAKE		
			10.0f,	//fall rate eANALOG_INPUT_HANDBRAKE	
			5.0f,	//fall rate eANALOG_INPUT_STEER_LEFT
			5.0f	//fall rate eANALOG_INPUT_STEER_RIGHT
		}
	};

	//debug mesh
	std::vector<Mesh> meshes;

};