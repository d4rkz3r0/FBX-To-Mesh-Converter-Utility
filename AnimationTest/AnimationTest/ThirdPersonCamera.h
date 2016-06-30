#pragma once
#include "Singleton.h"
#include "CommonIncludes.h"

using std::unique_ptr;
using std::make_unique;

class ThirdPersonCamera : public Singleton<ThirdPersonCamera>
{
public:
	friend class Singleton<ThirdPersonCamera>;
	ThirdPersonCamera();

	void Pie();

	//FreeLook
	static bool mFreeLookCamera;


	void Initialize(float aspectRatio, float FoV = XM_PIDIV4, float NearZ = 0.1f, float FarZ = 1000.0f, bool lockOnPlayer = true);
	void Update();
	void Shutdown();

	//Accessors
	XMFLOAT3 GetRight();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetPlayerPosition();
	XMFLOAT4 GetOrientation();
	XMFLOAT3 GetCameraTarget();

	//Mutators
	void SetRight(float x, float y, float z);
	void SetRight(const XMFLOAT3& right);
	void SetRightXM(const XMVECTOR& right);
	void SetUp(float x, float y, float z);
	void SetUp(const XMFLOAT3& up);
	void SetUpXM(const XMVECTOR& up);
	void SetLook(float x, float y, float z);
	void SetLook(const XMFLOAT3& look);
	void SetLookXM(const XMVECTOR& look);
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& position);
	void SetPlayerPosition(float x, float y, float z);
	void SetPlayerPosition(const XMFLOAT3& position);
	void SetPositionXM(const XMVECTOR& position);
	void SetOrientation(float xComponent, float yComponent, float zComponent, float wComponent);
	void SetOrientation(const XMFLOAT4& orientation);
	void SetOrientationXM(const XMVECTOR& orientation);
	void SetTarget(float x, float y, float z);
	void SetTarget(const XMFLOAT3& target);
	void SetTargetXM(const XMVECTOR& camTarget);
	void SetTargetYAxis(const XMFLOAT3& targetYAxis);
	void SetTargetYAxisXM(const XMVECTOR& yAxis);
	void SetOffsetDistance(float offsetDistance);

	//SIMD Accessors
	XMVECTOR GetRightXM();
	XMVECTOR GetUpXM();
	XMVECTOR GetLookXM();
	XMVECTOR GetPositionXM();
	XMVECTOR GetOrientationXM();
	XMVECTOR GetTargetXM();
	XMVECTOR GetTargetYAxisXM();
	XMMATRIX GetView();
	XMMATRIX GetProj();
	XMMATRIX GetViewProj();

	//Helper Funcs
	void SetFrustum(float foVY, float aspectRatio, float nearZ, float farZ);

	//Free Look Camera
	//Camera Transformation
	void VFly(float moveSpeed);
	void Strafe(float moveSpeed);
	void Step(float moveSpeed);
	void Pitch(float angle);
	void Yaw(float angle);
	void UpdateFPSViewMatrix();
	void FPSCamKeyboardInput();
	void ResetCameraTransform();

	//Third Person Camera
	void RotateCamera(float headingDegrees, float pitchDegrees);
	void LookAt(const XMFLOAT3& target);
	void LookAt(const XMFLOAT3& currCamPos, const XMFLOAT3& currTargetPos, const XMFLOAT3& currUpDir);
	void UpdateOrientation();
	void UpdateViewMatrix();


private:
	virtual ~ThirdPersonCamera() = default;
	ThirdPersonCamera(const ThirdPersonCamera& rhs) = delete;
	ThirdPersonCamera& operator=(const ThirdPersonCamera& rhs) = delete;

	//Camera's Local Coordinate System
	XMFLOAT3 mRight;
	XMFLOAT3 mUp;
	XMFLOAT3 mLook;
	XMFLOAT3 mPosition;
	XMFLOAT4 mOrientation;

	//Frustum's Properties
	float mFoV;
	float mAspectRatio;
	float mNearZ;
	float mFarZ;

	//Camera and it's Frustum
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

	//Camera Follow v1.0
	XMFLOAT3 mTarget;
	XMFLOAT3 mTargetYAxis;
	float mHeadingDegrees;
	float mPitchDegrees;
	float mOffsetDistance;
	const float mCameraRotationSpeed = 120.0f;
	const float mPlayerRadius = 60.0f;
	const float mCameraYOffsetScalar = 1.0f;
	const float mCameraZOffsetScalar = 2.5f;
	XMFLOAT3 mPlayerPosition;

	//Misc
	float mCameraSensitivity = 40.0f;
	float mCameraMovementRate = 200.0f;
	XMFLOAT3 mVelocity;
};